import sys
import os
import time

# 將 middleware 目錄加入 PATH
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

def main():
    try:
        import pyaxiom
        print("[✔] 成功導入 pyaxiom C++ 綁定模組")
    except ImportError as e:
        print(f"[✘] 無法導入 pyaxiom: {str(e)}")
        print("提示：請先使用 'pip install -e .' 編譯並安裝 C++ 核心")
        sys.exit(1)

    from axiom import SimulationAnalyzer, SimulationReporter

    # 1. 初始化模擬器 (預設 8 副牌，下注閒家 Player=1)
    # C++ constructor: PeekBaccaratSimulator(int shoe_count = 8, int default_bet = 1)
    sim = pyaxiom.PeekBaccaratSimulator(8, 1)
    sim.set_seed(42)

    # 2. 執行 1,000,000 局模擬
    rounds = 1000000
    print(f"[*] 開始執行 {rounds:,} 局 Monte Carlo 模擬...")
    start_time = time.time()
    raw_results = sim.run_simulation(rounds)
    end_time = time.time()
    print(f"[✔] 模擬完成，耗時: {end_time - start_time:.4f} 秒")
    print(f"[*] 平均每秒模擬: {rounds / (end_time - start_time):,.0f} 局")

    # 3. 中台 Pandas 分析
    print("[*] 啟動中台數據分析...")
    analyzer = SimulationAnalyzer(raw_results)
    metrics = analyzer.analyze_metrics()
    
    print("-" * 40)
    print("模擬核心指標分析結果:")
    print(f"  總局數: {metrics['total_rounds']:,}")
    print(f"  總投注主注: ${metrics['total_bet']:,.2f}")
    print(f"  總看牌手續費: ${metrics['total_fee']:,.2f}")
    print(f"  總額外加注: ${metrics['total_raise']:,.2f}")
    print(f"  實際玩家回報率 (RTP): {metrics['rtp']:.4%}")
    print(f"  波動率: {metrics['volatility']:.4f}")
    print(f"  最大資金回撤 (MDD): ${metrics['max_drawdown']:,.2f}")
    print(f"  閒贏佔比: {metrics['player_win_rate']:.2%}")
    print(f"  莊贏佔比: {metrics['banker_win_rate']:.2%}")
    print(f"  和局佔比: {metrics['tie_rate']:.2%}")
    print("-" * 40)

    # 4. 生成圖表與 Markdown 報告
    print("[*] 產生統計圖表與自動化 Markdown 報告...")
    trends = analyzer.get_trends(num_points=1000)
    reporter = SimulationReporter(metrics, trends)
    
    output_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "build", "reports"))
    chart_paths = reporter.generate_charts(output_dir, prefix="test_")
    
    report_path = os.path.join(output_dir, "simulation_report.md")
    reporter.generate_markdown_report(chart_paths, report_path)
    
    print(f"[✔] 報告生成成功：{report_path}")

if __name__ == "__main__":
    main()
