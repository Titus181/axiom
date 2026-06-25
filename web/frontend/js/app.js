// 全域圖表實例變數，便於重繪前銷毀
let rtpChartInstance = null;
let balanceChartInstance = null;

document.addEventListener("DOMContentLoaded", () => {
    initTabs();
    initForm();
});

// 1. 初始化分頁切換
function initTabs() {
    const tabButtons = document.querySelectorAll(".tab-btn");
    const tabContents = document.querySelectorAll(".tab-content");

    tabButtons.forEach(btn => {
        btn.addEventListener("click", () => {
            const targetTab = btn.getAttribute("data-tab");

            tabButtons.forEach(b => b.classList.remove("active"));
            tabContents.forEach(c => c.classList.remove("active"));

            btn.classList.add("active");
            document.getElementById(targetTab).classList.add("active");
        });
    });
}

// 2. 表單提交與模擬觸發
function initForm() {
    const btnRun = document.getElementById("btn-run");
    const spinner = document.getElementById("sim-spinner");

    btnRun.addEventListener("click", async () => {
        // 取得表單參數
        const gameType = document.getElementById("game-type").value;
        const rounds = parseInt(document.getElementById("rounds").value);
        const seed = parseInt(document.getElementById("seed").value);
        const betType = parseInt(document.querySelector('input[name="bet_type"]:checked').value);

        // 設置 Loading 狀態
        btnRun.disabled = true;
        spinner.style.display = "block";
        document.querySelector(".btn-text").textContent = "模擬計算中...";

        try {
            const response = await fetch("/api/simulate", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    game_type: gameType,
                    rounds: rounds,
                    bet_type: betType,
                    seed: seed
                })
            });

            if (!response.ok) {
                const errorData = await response.json();
                throw new Error(errorData.detail || "模擬執行失敗");
            }

            const data = await response.json();
            
            // 1. 更新卡片數據
            updateMetrics(data.metrics);

            // 2. 渲染 Chart.js 圖表
            renderCharts(data.trends);

            // 3. 讀取並渲染 Markdown 報告
            await loadAndRenderReport(data.report_url);

        } catch (error) {
            alert(`執行模擬出錯: ${error.message}`);
        } finally {
            // 恢復按鈕狀態
            btnRun.disabled = false;
            spinner.style.display = "none";
            document.querySelector(".btn-text").textContent = "開始數值模擬";
        }
    });
}

// 3. 更新首頁指標卡片
function updateMetrics(m) {
    document.getElementById("val-rtp").textContent = (m.rtp * 100).toFixed(4) + "%";
    
    const profitEl = document.getElementById("val-profit");
    const profit = m.total_win_loss;
    profitEl.textContent = (profit >= 0 ? "+" : "") + profit.toLocaleString(undefined, {minimumFractionDigits: 2, maximumFractionDigits: 2});
    
    // 如果贏錢則綠色，輸錢則紅色
    if (profit >= 0) {
        profitEl.style.color = "#10b981";
    } else {
        profitEl.style.color = "#ef4444";
    }

    document.getElementById("val-turnover").textContent = `總投入資金: $${m.total_turnover.toLocaleString(undefined, {minimumFractionDigits: 2, maximumFractionDigits: 2})}`;
    document.getElementById("val-volatility").textContent = m.volatility.toFixed(4);
    document.getElementById("val-mdd").textContent = `$${m.max_drawdown.toLocaleString(undefined, {minimumFractionDigits: 2, maximumFractionDigits: 2})}`;
}

// 4. 使用 Chart.js 畫圖
function renderCharts(trends) {
    // 銷毀舊實例
    if (rtpChartInstance) rtpChartInstance.destroy();
    if (balanceChartInstance) balanceChartInstance.destroy();

    const commonOptions = {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                labels: { color: "#8b9bb4" }
            }
        },
        scales: {
            x: {
                grid: { color: "rgba(255, 255, 255, 0.05)" },
                ticks: { color: "#8b9bb4" }
            },
            y: {
                grid: { color: "rgba(255, 255, 255, 0.05)" },
                ticks: { color: "#8b9bb4" }
            }
        }
    };

    // 4.1 RTP 收斂圖
    const ctxRtp = document.getElementById("chart-rtp").getContext("2d");
    rtpChartInstance = new Chart(ctxRtp, {
        type: "line",
        data: {
            labels: trends.rounds,
            datasets: [
                {
                    label: "模擬 RTP (%)",
                    data: trends.rtp.map(v => v * 100),
                    borderColor: "#3b82f6",
                    borderWidth: 2,
                    pointRadius: 0,
                    fill: false
                },
                {
                    label: "理論期望 RTP (98.80%)",
                    data: Array(trends.rounds.length).fill(98.80),
                    borderColor: "#ef4444",
                    borderDash: [5, 5],
                    pointRadius: 0,
                    fill: false
                }
            ]
        },
        options: commonOptions
    });

    // 4.2 資產變化圖
    const ctxBalance = document.getElementById("chart-balance").getContext("2d");
    balanceChartInstance = new Chart(ctxBalance, {
        type: "line",
        data: {
            labels: trends.rounds,
            datasets: [{
                label: "累計淨損益 (Win/Loss)",
                data: trends.balance,
                borderColor: "#10b981",
                borderWidth: 2,
                pointRadius: 0,
                fill: true,
                backgroundColor: "rgba(16, 185, 129, 0.05)"
            }]
        },
        options: commonOptions
    });
}

// 5. 載入並解析 Markdown 報告
async function loadAndRenderReport(url) {
    try {
        const response = await fetch(url);
        if (!response.ok) throw new Error("無法獲取 Markdown 報告");
        
        const data = await response.json();
        
        // 渲染 Markdown (將內置的 md 轉換為 HTML)
        const viewer = document.getElementById("report-viewer");
        viewer.innerHTML = marked.parse(data.content);
        
    } catch (err) {
        console.error("渲染報告出錯:", err);
    }
}
