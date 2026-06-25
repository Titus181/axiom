#pragma once
#include <vector>
#include <string>
#include <memory>
#include "rng.hpp"

namespace axiom {

// 卡牌花色
enum class Suit {
    Spades,
    Hearts,
    Diamonds,
    Clubs
};

// 卡牌結構
struct Card {
    Suit suit;
    int rank; // 1=A, 2-10, 11=J, 12=Q, 13=K
    
    // 取得百家樂的點數 (A=1, 2-9=原點數, 10,J,Q,K=0)
    [[nodiscard]] int GetBaccaratValue() const {
        if (rank >= 10) return 0;
        return rank;
    }

    [[nodiscard]] std::string ToString() const {
        std::string rank_str;
        if (rank == 1) rank_str = "A";
        else if (rank == 11) rank_str = "J";
        else if (rank == 12) rank_str = "Q";
        else if (rank == 13) rank_str = "K";
        else rank_str = std::to_string(rank);

        switch (suit) {
            case Suit::Spades: return "♠" + rank_str;
            case Suit::Hearts: return "♥" + rank_str;
            case Suit::Diamonds: return "♦" + rank_str;
            case Suit::Clubs: return "♣" + rank_str;
        }
        return "";
    }
};

// 牌靴類別
class Shoe {
public:
    // 初始化指定副數的牌靴 (通常百家樂為 8 副)
    Shoe(int num_decks, std::shared_ptr<RandomNumberGenerator> rng);

    // 重新洗牌
    void Shuffle();

    // 發一張牌
    Card Draw();

    // 取得剩餘張數
    [[nodiscard]] int RemainingCards() const { return cards_.size() - current_index_; }

    // 是否需要重新洗牌 (當剩餘牌數低於切牌線，預設 60 張)
    [[nodiscard]] bool NeedsReshuffle(int cut_card_limit = 60) const {
        return RemainingCards() <= cut_card_limit;
    }

private:
    int num_decks_;
    std::shared_ptr<RandomNumberGenerator> rng_;
    std::vector<Card> cards_;
    int current_index_;

    void InitializeDeck();
};

} // namespace axiom
