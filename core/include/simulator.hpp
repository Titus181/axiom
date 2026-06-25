#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace axiom {

// 統一的對局結果結構
struct SimulationResult {
    uint64_t round_id;      // 對局 ID
    double bet_amount;     // 初始下注額 (本金)
    double peek_fee;       // 強制手續費 (主注 20%)
    double raise_amount;   // 加注額 (0, 2 * bet, 3 * bet)
    double win_loss;       // 淨損益 (派彩 - 總投入資金)
    int game_status;       // 狀態碼 (例如：0=莊贏, 1=閒贏, 2=和局)
    std::string detail;    // JSON 格式的詳細資訊，以利中台解析或前端渲染
};

class BaseSimulator {
public:
    virtual ~BaseSimulator() = default;

    // 核心模擬介面
    virtual std::vector<SimulationResult> RunSimulation(uint64_t rounds) = 0;

    // 設定種子，保證隨機性可被重現
    virtual void SetSeed(uint64_t seed) = 0;
};

} // namespace axiom
