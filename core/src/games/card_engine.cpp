#include "games/card_engine.hpp"

namespace axiom {

Shoe::Shoe(int num_decks, std::shared_ptr<RandomNumberGenerator> rng)
    : num_decks_(num_decks), rng_(rng), current_index_(0) {
    InitializeDeck();
    Shuffle();
}

void Shoe::InitializeDeck() {
    cards_.clear();
    cards_.reserve(num_decks_ * 52);
    
    for (int d = 0; d < num_decks_; ++d) {
        for (int s = 0; s < 4; ++s) {
            Suit suit = static_cast<Suit>(s);
            for (int r = 1; r <= 13; ++r) {
                cards_.push_back(Card{suit, r});
            }
        }
    }
}

void Shoe::Shuffle() {
    rng_->Shuffle(cards_);
    current_index_ = 0;
}

Card Shoe::Draw() {
    if (current_index_ >= cards_.size()) {
        // 安全機制：若發完牌則自動重新洗牌
        Shuffle();
    }
    return cards_[current_index_++];
}

} // namespace axiom
