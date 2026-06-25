#include "games/peek_baccarat.hpp"
#include <sstream>
#include <numeric>
#include <iomanip>
#include <algorithm>

namespace axiom {

PeekBaccaratSimulator::PeekBaccaratSimulator(int shoe_count, BetType default_bet)
    : shoe_count_(shoe_count), default_bet_(default_bet), reveal_weights_({0.59, 0.16, 0.15, 0.10}) {
    rng_ = std::make_shared<RandomNumberGenerator>();
    shoe_ = std::make_unique<Shoe>(shoe_count_, rng_);
}

void PeekBaccaratSimulator::SetSeed(uint64_t seed) {
    rng_->SetSeed(seed);
    shoe_->Shuffle();
}

void PeekBaccaratSimulator::SetBetType(int bet_type) {
    default_bet_ = static_cast<BetType>(bet_type);
}

int PeekBaccaratSimulator::GetBetType() const {
    return static_cast<int>(default_bet_);
}

void PeekBaccaratSimulator::SetRevealWeights(const std::vector<double>& weights) {
    if (weights.size() == 4) {
        reveal_weights_ = weights;
    }
}

std::vector<SimulationResult> PeekBaccaratSimulator::RunSimulation(uint64_t rounds) {
    std::vector<SimulationResult> results;
    results.reserve(rounds);

    // 每一億局前重新洗牌，或在剩餘牌數不夠時洗牌
    shoe_->Shuffle();

    for (uint64_t i = 1; i <= rounds; ++i) {
        if (shoe_->NeedsReshuffle()) {
            shoe_->Shuffle();
        }
        results.push_back(PlayOneRound(i));
    }

    return results;
}

std::pair<int, int> PeekBaccaratSimulator::EvaluateTableau(std::vector<Card>& player_hand, std::vector<Card>& banker_hand) {
    int p_sum = (player_hand[0].GetBaccaratValue() + player_hand[1].GetBaccaratValue()) % 10;
    int b_sum = (banker_hand[0].GetBaccaratValue() + banker_hand[1].GetBaccaratValue()) % 10;

    // 判斷是否為天生贏家 (Natural: 閒或莊起手 8 或 9 點)
    if (p_sum >= 8 || b_sum >= 8) {
        return {p_sum, b_sum};
    }

    // 閒家補牌規則
    bool player_drew = false;
    Card player_third_card{};
    if (p_sum <= 5) {
        player_third_card = shoe_->Draw();
        player_hand.push_back(player_third_card);
        p_sum = (p_sum + player_third_card.GetBaccaratValue()) % 10;
        player_drew = true;
    }

    // 莊家補牌規則
    if (!player_drew) {
        // 若閒家停牌，莊家若為 0-5 補牌，6-7 停牌
        if (b_sum <= 5) {
            banker_hand.push_back(shoe_->Draw());
            b_sum = (b_sum + banker_hand.back().GetBaccaratValue()) % 10;
        }
    } else {
        // 若閒家補了第三張牌，莊家依下表補牌
        int p_third_val = player_third_card.GetBaccaratValue();
        bool banker_draw = false;

        switch (b_sum) {
            case 0:
            case 1:
            case 2:
                banker_draw = true;
                break;
            case 3:
                // 閒家第三張不是 8 則補
                if (p_third_val != 8) banker_draw = true;
                break;
            case 4:
                // 閒家第三張是 2-7 則補
                if (p_third_val >= 2 && p_third_val <= 7) banker_draw = true;
                break;
            case 5:
                // 閒家第三張是 4-7 則補
                if (p_third_val >= 4 && p_third_val <= 7) banker_draw = true;
                break;
            case 6:
                // 閒家第三張是 6-7 則補
                if (p_third_val == 6 || p_third_val == 7) banker_draw = true;
                break;
            default:
                // 7, 8, 9 停牌
                banker_draw = false;
                break;
        }

        if (banker_draw) {
            banker_hand.push_back(shoe_->Draw());
            b_sum = (b_sum + banker_hand.back().GetBaccaratValue()) % 10;
        }
    }

    return {p_sum, b_sum};
}

int PeekBaccaratSimulator::MakeDecision(const std::vector<Card>& player_hand, 
                                        const std::vector<Card>& banker_hand,
                                        const std::vector<bool>& player_revealed,
                                        const std::vector<bool>& banker_revealed) {
    // 取得已揭露的卡牌點數
    int p_rev_sum = 0;
    int p_rev_count = 0;
    for (size_t i = 0; i < 2; ++i) {
        if (player_revealed[i]) {
            p_rev_sum += player_hand[i].GetBaccaratValue();
            p_rev_count++;
        }
    }

    int b_rev_sum = 0;
    int b_rev_count = 0;
    for (size_t i = 0; i < 2; ++i) {
        if (banker_revealed[i]) {
            b_rev_sum += banker_hand[i].GetBaccaratValue();
            b_rev_count++;
        }
    }

    p_rev_sum %= 10;
    b_rev_sum %= 10;

    // 決策邏輯：根據投注項目做期望值評估
    if (default_bet_ == BetType::Player) {
        // 下注閒家
        if (p_rev_count > 0 && b_rev_count > 0) {
            int diff = p_rev_sum - b_rev_sum;
            // 閒家揭露點數明顯大於莊家時加注 3 倍
            if (diff >= 3) return 3;
            if (diff >= 1) return 2;
        } else if (p_rev_count > 0 && p_rev_sum >= 8) {
            // 只有閒家揭露且大於 8 點 (很高機率是 natural 8 或 9)
            return 3;
        } else if (b_rev_count > 0 && b_rev_sum == 0) {
            // 莊家揭露點數是 0 點 (極有可能最終點數不高)
            return 2;
        }
    } else {
        // 下注莊家
        if (p_rev_count > 0 && b_rev_count > 0) {
            int diff = b_rev_sum - p_rev_sum;
            // 莊家揭露點數明顯大於閒家時加注 3 倍
            if (diff >= 3) return 3;
            if (diff >= 1) return 2;
        } else if (b_rev_count > 0 && b_rev_sum >= 8) {
            // 莊家揭露大牌
            return 3;
        } else if (p_rev_count > 0 && p_rev_sum == 0) {
            // 閒家揭露 0 點
            return 2;
        }
    }

    return 0; // Check
}

SimulationResult PeekBaccaratSimulator::PlayOneRound(uint64_t round_id) {
    // 1. 初始投注與看牌手續費
    const double base_bet = 100.0;
    const double peek_fee = base_bet * 0.20; // 20% 強制手續費

    // 2. 發初始四張牌 (閒家 2 張，莊家 2 張)
    std::vector<Card> player_hand = { shoe_->Draw(), shoe_->Draw() };
    std::vector<Card> banker_hand = { shoe_->Draw(), shoe_->Draw() };

    // 3. 隨機揭牌機制 (根據 reveal_weights_ 權重隨機揭露 1 到 4 張牌)
    double r = rng_->NextDouble();
    double sum = 0.0;
    for (double w : reveal_weights_) sum += w;
    double acc = 0.0;
    int num_to_reveal = 1;
    for (size_t i = 0; i < reveal_weights_.size(); ++i) {
        acc += reveal_weights_[i] / sum;
        if (r <= acc) {
            num_to_reveal = i + 1;
            break;
        }
    }
    
    // 建立 4 個位置的索引，並洗牌決定揭露哪些位置
    std::vector<int> positions = { 0, 1, 2, 3 }; // 0,1 為閒家；2,3 為莊家
    rng_->Shuffle(positions);

    std::vector<bool> player_revealed(2, false);
    std::vector<bool> banker_revealed(2, false);

    for (int i = 0; i < num_to_reveal; ++i) {
        int pos = positions[i];
        if (pos < 2) {
            player_revealed[pos] = true;
        } else {
            banker_revealed[pos - 2] = true;
        }
    }

    // 4. 加注決策 (0x, 2x, 或 3x)
    int multiplier = MakeDecision(player_hand, banker_hand, player_revealed, banker_revealed);
    double raise_amount = base_bet * multiplier;

    // 5. 補牌與計算最終結果 (注意：EvaluateTableau 可能會增發牌到 player_hand/banker_hand)
    auto [p_final, b_final] = EvaluateTableau(player_hand, banker_hand);

    // 6. 結算勝負
    int winner = 2; // 0=庄, 1=閒, 2=和
    if (p_final > b_final) {
        winner = 1;
    } else if (b_final > p_final) {
        winner = 0;
    }

    // 7. 計算淨損益 (Win/Loss)
    // 投注本金加加注金額 (手續費不作為贏錢計算的本金，且手續費在贏輸和時的處理規則見規格書：
    // 贏：退本金 + 派彩 (1:1 或 1:0.95)，手續費不退。
    // 輸：本金與加注全輸，手續費不退。
    // 和局：主注及加注金額退回，但 20% 手續費不予退回。
    double win_loss = 0.0;
    double total_invested = base_bet + peek_fee + raise_amount;

    if (default_bet_ == BetType::Player) {
        // 投注閒家
        if (winner == 1) {
            // 閒贏：派彩 = 1:1 初始主注 + 1:1 加注
            win_loss = (base_bet + raise_amount) - peek_fee; 
        } else if (winner == 0) {
            // 莊贏 (玩家輸)：輸掉主注與加注
            win_loss = -total_invested;
        } else {
            // 和局：退回主注與加注，損失手續費
            win_loss = -peek_fee;
        }
    } else {
        // 投注莊家
        if (winner == 0) {
            // 莊贏：派彩 = 0.95 初始主注 + 0.95 加注
            win_loss = (base_bet + raise_amount) * 0.95 - peek_fee;
        } else if (winner == 1) {
            // 閒贏 (玩家輸)
            win_loss = -total_invested;
        } else {
            // 和局
            win_loss = -peek_fee;
        }
    }

    // 8. 組裝詳細日誌資訊 (JSON 格式)
    std::stringstream detail_ss;
    detail_ss << "{";
    detail_ss << "\"player_initial\":\"" << player_hand[0].ToString() << "," << player_hand[1].ToString() << "\",";
    detail_ss << "\"banker_initial\":\"" << banker_hand[0].ToString() << "," << banker_hand[1].ToString() << "\",";
    detail_ss << "\"revealed\":\"P:" << (player_revealed[0] ? "Y" : "N") << (player_revealed[1] ? "Y" : "N") 
              << "|B:" << (banker_revealed[0] ? "Y" : "N") << (banker_revealed[1] ? "Y" : "N") << "\",";
    detail_ss << "\"decision_multiplier\":" << multiplier << ",";
    detail_ss << "\"player_final_cards\":\"";
    for (size_t i = 0; i < player_hand.size(); ++i) {
        detail_ss << player_hand[i].ToString() << (i == player_hand.size() - 1 ? "" : ",");
    }
    detail_ss << "\",\"banker_final_cards\":\"";
    for (size_t i = 0; i < banker_hand.size(); ++i) {
        detail_ss << banker_hand[i].ToString() << (i == banker_hand.size() - 1 ? "" : ",");
    }
    detail_ss << "\",\"player_final_points\":" << p_final << ",";
    detail_ss << "\"banker_final_points\":" << b_final;
    detail_ss << "}";

    SimulationResult res;
    res.round_id = round_id;
    res.bet_amount = base_bet;
    res.peek_fee = peek_fee;
    res.raise_amount = raise_amount;
    res.win_loss = win_loss;
    res.game_status = winner;
    res.detail = detail_ss.str();

    return res;
}

} // namespace axiom
