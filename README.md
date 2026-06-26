# Project Axiom (公理數值核心系統)

Project Axiom 是一個「機制與業務分離」的高效能通用遊戲數值洗鍊與模擬分析中台。

## 系統架構

Axiom 平台採用三層結構設計：
1. **底層運算層 (`core/`)**：基於 C++20 開發，專注於高效能 Monte Carlo 模擬、RNG 偽隨機數消解及遊戲核心邏輯（第一階段實作看牌百家樂 `Peek Baccarat` 引擎），不含任何業務展示層。
2. **數據中台層 (`middleware/`)**：透過 `pybind11` 將 C++ 核心封裝為 Python 模組，並使用 `Pandas` 與 `Numpy` 進行大數據量的數值分析（如 RTP 收斂、波動率與最大回撤），最終自動生成分析報告。
3. **服務與前端展示層 (`web/`)**：後端採用 `FastAPI` 提供非同步模擬任務調度 API，前端使用精美的 Web 控制台展示即時模擬狀態與渲染 Markdown 數據報告。

## 目錄結構

```
Axiom/
├── docs/                               # 規格文件
├── core/                               # C++ 底層運算層
│   ├── CMakeLists.txt                  # 構建檔
│   ├── include/                        # 標頭檔 (RNG, BaseSimulator, CardEngine, PeekBaccarat)
│   ├── src/                            # 原始碼
│   └── binding/                        # pybind11 綁定程式
├── middleware/                         # Python 中台層
│   ├── setup.py                        # 整合編譯安裝腳本
│   ├── requirements.txt                # 依賴庫
│   └── axiom/                          # 數據分析與報告生成
├── web/                                # 服務與展示層
│   ├── backend/                        # FastAPI 後端
│   └── frontend/                       # Web 前端控制台
└── README.md
```

## 開發與編譯指南

請參考以下說明文件以進行編譯與啟動：
* 🚀 **[快速啟動指南 (Quickstart)](file:///Users/titus/Documents/GitHub/Axiom/docs/QUICKSTART.md)**：僅需幾步即可快速執行專案服務。
* 📖 **[網頁服務啟動說明書 (Startup Guide)](file:///Users/titus/Documents/GitHub/Axiom/docs/STARTUP.md)**：包含 C++ 核心編譯、Python 依賴安裝、Vue 3 前端開發與生產打包的詳細環境配置指引。

