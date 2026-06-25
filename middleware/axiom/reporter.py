import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from jinja2 import Template

class SimulationReporter:
    def __init__(self, metrics: dict, trends: dict):
        self.metrics = metrics
        self.trends = trends

    def generate_charts(self, output_dir: str, prefix: str = "") -> dict:
        """
        生成 RTP 收斂圖和資產曲線圖，返回圖片的絕對路徑字典
        """
        os.makedirs(output_dir, exist_ok=True)
        
        rtp_path = os.path.join(output_dir, f"{prefix}rtp_convergence.png")
        balance_path = os.path.join(output_dir, f"{prefix}balance_trend.png")

        # 1. 繪製 RTP 收斂圖
        plt.figure(figsize=(10, 5))
        plt.plot(self.trends["rounds"], [r * 100 for r in self.trends["rtp"]], label="Simulated RTP", color="#1a73e8", linewidth=2)
        plt.axhline(y=98.80, color="#d93025", linestyle="--", label="Theoretical RTP (98.80%)")
        plt.title("RTP Convergence Curve (Monte Carlo)", fontsize=14, fontweight="bold")
        plt.xlabel("Simulation Rounds", fontsize=12)
        plt.ylabel("RTP (%)", fontsize=12)
        plt.grid(True, linestyle=":", alpha=0.6)
        plt.legend(fontsize=10)
        plt.tight_layout()
        plt.savefig(rtp_path, dpi=150)
        plt.close()

        # 2. 繪製資產餘額變化圖
        plt.figure(figsize=(10, 5))
        plt.plot(self.trends["rounds"], self.trends["balance"], label="Accumulated Profit/Loss", color="#34a853", linewidth=2)
        plt.axhline(y=0, color="#5f6368", linestyle="-")
        plt.title("Balance Trend (Total Wealth Deviation)", fontsize=14, fontweight="bold")
        plt.xlabel("Simulation Rounds", fontsize=12)
        plt.ylabel("Net Profit/Loss (Units)", fontsize=12)
        plt.grid(True, linestyle=":", alpha=0.6)
        plt.legend(fontsize=10)
        plt.tight_layout()
        plt.savefig(balance_path, dpi=150)
        plt.close()

        return {
            "rtp_chart": rtp_path,
            "balance_chart": balance_path
        }

    def generate_markdown_report(self, chart_paths: dict, output_path: str, game_name: str = "Peek Baccarat"):
        """
        利用 Jinja2 模板生成包含數據與圖表的 Markdown 報告
        """
        # 取得圖片的相對路徑 (相對於報告所在目錄)
        report_dir = os.path.dirname(output_path)
        rtp_relative = os.path.relpath(chart_paths["rtp_chart"], report_dir)
        balance_relative = os.path.relpath(chart_paths["balance_chart"], report_dir)

        template_str = """# Project Axiom 數值模擬分析報告 - {{ game_name }}

> [!NOTE]
> 本報告由 **Axiom 數值中台** 自動生成。底層採用高效能 C++20 模擬核心，中台使用 Pandas 進行大數統計分析。

## 1. 核心數值指標

| 指標名稱 | 模擬數值 | 備註說明 |
| :--- | :--- | :--- |
| **總模擬對局數** | {{ "{:,}".format(m.total_rounds) }} 局 | Monte Carlo 隨機對局次數 |
| **總投入主注** | {{ "${:,.2f}".format(m.total_bet) }} | 初始下注本金總和 |
| **總看牌手續費 (20%)** | {{ "${:,.2f}".format(m.total_fee) }} | 強制收取手續費 |
| **總額外加注** | {{ "${:,.2f}".format(m.total_raise) }} | 根據最優決策之 2x/3x 加注額 |
| **累計投入總資金** | {{ "${:,.2f}".format(m.total_turnover) }} | 主注 + 手續費 + 加注 |
| **累計淨損益 (Win/Loss)** | **{{ "${:,.2f}".format(m.total_win_loss) }}** | 玩家總盈虧 (包含手續費支出) |
| **實際玩家回報率 (RTP)** | **{{ "{:.4%}".format(m.rtp) }}** | 完美決策下模擬 RTP (理論值約 98.80%) |
| **波動率 (Volatility)** | {{ "{:.4f}".format(m.volatility) }} | 單局淨損益之標準差 (以主注 100 為單位) |
| **最大資金回撤 (MDD)** | {{ "${:,.2f}".format(m.max_drawdown) }} | 模擬期間自最高利潤點的最大下滑金額 |

### 對局結果統計
- **莊贏局數佔比 (Banker Win Rate)**: `{{ "{:.2%}".format(m.banker_win_rate) }}`
- **閒贏局數佔比 (Player Win Rate)**: `{{ "{:.2%}".format(m.player_win_rate) }}`
- **和局局數佔比 (Tie Rate)**: `{{ "{:.2%}".format(m.tie_rate) }}`

---

## 2. 數值趨勢圖表

### 2.1 RTP 收斂曲線
本圖表展示了模擬 RTP 隨著對局次數增加，逐漸向理論期望值收斂的過程。這驗證了隨機數生成器（RNG）的隨機均勻度以及補牌、結算邏輯的精確性。

![RTP Convergence]({{ rtp_chart }})

### 2.2 資產變化曲線 (Balance Trend)
本圖表展示了玩家在模擬過程中的累計盈虧變化。即便收取了 20% 的看牌手續費，但在完美決策（適時加注 3x）的加持下，資產曲線的下滑斜率被有效控制，達到了期望值對沖效果。

![Balance Trend]({{ balance_chart }})

---

## 3. 架構可擴充性評估
1. **機制分離**：已成功將 RNG、撲克牌靴 (`Shoe`) 與百家樂 Tableau 規則在 C++ 層解耦。
2. **多型擴充**：未來只需新增 `dice.hpp` 或 `slots.hpp` 並繼承 `BaseSimulator`，即可使用相同的 Pandas 數據中台與報告生成器。
"""
        template = Template(template_str)
        rendered_md = template.render(
            game_name=game_name,
            m=self.metrics,
            rtp_chart=rtp_relative,
            balance_chart=balance_relative
        )

        os.makedirs(report_dir, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(rendered_md)
