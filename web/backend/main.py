import os
import sys
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from pydantic import BaseModel

# 將中台目錄加入 Python 搜尋路徑
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "middleware")))

try:
    import pyaxiom
    from axiom import SimulationAnalyzer, SimulationReporter
except ImportError:
    # 支援在尚未編譯安裝時加載，避免 FastAPI 初始化直接崩潰
    pyaxiom = None
    SimulationAnalyzer = None
    SimulationReporter = None

app = FastAPI(title="Project Axiom 數值模擬中台", version="1.0.0")

# CORS 支持
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# 報告與圖表儲存目錄
REPORTS_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "build", "reports"))
os.makedirs(REPORTS_DIR, exist_ok=True)

class SimulationRequest(BaseModel):
    game_type: str = "peek_baccarat"
    rounds: int = 100000
    bet_type: int = 1  # 0=Banker, 1=Player
    seed: int = 42

@app.post("/api/simulate")
def run_simulation(req: SimulationRequest):
    if pyaxiom is None:
        raise HTTPException(status_code=500, detail="C++ core library (pyaxiom) is not compiled. Please run setup.py first.")

    if req.game_type != "peek_baccarat":
        raise HTTPException(status_code=400, detail="Unsupported game type")
        
    try:
        # 1. 執行 C++ 模擬
        sim = pyaxiom.PeekBaccaratSimulator(8, req.bet_type)
        sim.set_seed(req.seed)
        raw_results = sim.run_simulation_summary(req.rounds, 300)
        
        # 2. 進行數據分析
        analyzer = SimulationAnalyzer(raw_results)
        metrics = analyzer.analyze_metrics()
        trends = analyzer.get_trends(num_points=300) # 前端繪圖取 300 點即可
        
        # 3. 產生 Markdown 報告與圖表
        reporter = SimulationReporter(metrics, trends)
        chart_paths = reporter.generate_charts(REPORTS_DIR, prefix=f"sim_{req.seed}_")
        
        report_filename = f"report_{req.seed}.md"
        report_path = os.path.join(REPORTS_DIR, report_filename)
        reporter.generate_markdown_report(chart_paths, report_path)
        
        return {
            "status": "success",
            "metrics": metrics,
            "trends": trends,
            "report_url": f"/api/reports/{report_filename}",
            "charts": {
                "rtp": f"/api/reports/{os.path.basename(chart_paths['rtp_chart'])}",
                "balance": f"/api/reports/{os.path.basename(chart_paths['balance_chart'])}"
            }
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# 獲取報告內容或圖表
@app.get("/api/reports/{filename}")
def get_report(filename: str):
    path = os.path.join(REPORTS_DIR, filename)
    if not os.path.exists(path):
        raise HTTPException(status_code=404, detail="File not found")
    
    if filename.endswith(".md"):
        with open(path, "r", encoding="utf-8") as f:
            return {"content": f.read()}
    else:
        return FileResponse(path)

# 掛載靜態文件夾 (前端展示)
frontend_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "frontend"))
if os.path.exists(frontend_dir):
    app.mount("/", StaticFiles(directory=frontend_dir, html=True), name="frontend")
