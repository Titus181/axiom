#pragma once
#include "simulator.hpp"
#include "games/card_engine.hpp"
#include <memory>
#include <string>

namespace axiom {

// 投注選項 (主要投注項)
enum class BetType {
    Banker = 0,
    Player = 1
};

class PeekBaccaratSimulator : public BaseSimulator {
public:
    explicit PeekBaccaratSimulator(int shoe_count = 8, BetType default_bet = BetType::Player);

    // 實作 BaseSimulator 介面
    std::vector<SimulationResult> RunSimulation(uint64_t rounds) override;
    void SetSeed(uint64_t seed) override;

    // 設定與取得投注類型 (在 Python 中可調)
    void SetBetType(int bet_type);
    [[nodiscard]] int GetBetType() const;

private:
    int shoe_count_;
    BetType default_bet_;
    std::shared_ptr<RandomNumberGenerator> rng_;
    std::unique_ptr<Shoe> shoe_;

    // 核心單局執行邏輯
    SimulationResult PlayOneRound(uint64_t round_id);

    // 補牌邏輯 (Tableau)
    // 傳回值為莊家與閒家的最終點數，第一個為閒點數，第二個為莊點數，並且把發的牌記錄在傳入的 vectors 中
    std::pair<int, int> EvaluateTableau(std::vector<Card>& player_hand, std::vector<Card>& banker_hand);

    // 決策邏輯：根據已揭露的牌資訊決定加注倍率 (0=不加注/Check, 2=加注2x, 3=加注3x)
    int MakeDecision(const std::vector<Card>& player_hand, 
                     const std::vector<Card>& banker_hand,
                     const std::vector<bool>& player_revealed,
                     const std::vector<bool>& banker_revealed);
};

} // namespace axiom
