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

struct SimulationSummary {
    uint64_t total_rounds;
    double total_bet;
    double total_fee;
    double total_raise;
    double total_turnover;
    double total_win_loss;
    double rtp;
    double volatility;
    double max_drawdown;
    
    // 降採樣後的趨勢點資料 (限額 num_trend_points)
    std::vector<uint64_t> trend_rounds;
    std::vector<double> trend_rtps;
    std::vector<double> trend_balances;
    
    // 各勝負佔比
    uint64_t banker_wins;
    uint64_t player_wins;
    uint64_t ties;
};

class BaseSimulator {
public:
    virtual ~BaseSimulator() = default;

    // 舊的核心模擬介面 (保留用於小批量或單局調試)
    virtual std::vector<SimulationResult> RunSimulation(uint64_t rounds) = 0;

    // 大數據專用模擬介面 (O(1) 空間複雜度，防止記憶體溢出)
    virtual SimulationSummary RunSimulationSummary(uint64_t rounds, int num_trend_points = 500) = 0;

    // 設定種子，保證隨機性可被重現
    virtual void SetSeed(uint64_t seed) = 0;
};

} // namespace axiom
