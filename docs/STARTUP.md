# Project Axiom 網頁服務啟動說明書

本說明書引導您如何在本地環境編譯 C++ 模擬核心、安裝 Python 中台依賴，並啟動基於 FastAPI 的網頁服務與前端控制台。

---

## 系統架構簡介

Project Axiom 採用**機制與業務分離**的三層架構：
1. **C++ 核心運算層 (`core/`)**：高效能 Monte Carlo 模擬與遊戲邏輯（看牌百家樂）。
2. **Python 中台層 (`middleware/`)**：使用 `pybind11` 將 C++ 封裝為 `pyaxiom` 模組，並進行數據分析與報告生成。
3. **Web 展示層 (`web/`)**：[FastAPI 後端 API](file:///Users/titus/Documents/GitHub/Axiom/web/backend/main.py) 與 HTML5/CSS3/JS [前端控制台](file:///Users/titus/Documents/GitHub/Axiom/web/frontend/index.html)。

---

## 1. 系統需求與環境準備

在開始之前，請確保您的系統已安裝以下工具：

### 1.1 C++ 編譯器 (支援 C++20 標準)
* **macOS**: 安裝 Xcode 命令行工具：
  ```bash
  xcode-select --install
  ```
* **Linux (Ubuntu/Debian)**: 安裝 `build-essential` 與 `cmake`：
  ```bash
  sudo apt-get update
  sudo apt-get install build-essential cmake
  ```
* **Windows**: 安裝 Visual Studio 2019 或更新版本（須勾選「使用 C++ 的桌面開發」）。

### 1.2 Python 環境
* Python 3.8 或以上版本（推薦與本專案已構建環境一致的 Python 3.13）。

---

## 2. 安裝與編譯步驟

請依序在專案根目錄下執行以下步驟：

### 步驟 2.1：啟用 Python 虛擬環境
專案根目錄已附帶虛擬環境 `.venv`，請直接啟用：
* **macOS / Linux**:
  ```bash
  source .venv/bin/activate
  ```
* **Windows (Command Prompt)**:
  ```cmd
  .venv\Scripts\activate.bat
  ```
* **Windows (PowerShell)**:
  ```powershell
  .venv\Scripts\Activate.ps1
  ```

### 步驟 2.2：安裝 Python 依賴套件
在啟動的虛擬環境中，安裝所有中台與後端所需的依賴項目（包含 `pybind11`、`pandas`、`fastapi`、`uvicorn` 等，詳見 [requirements.txt](file:///Users/titus/Documents/GitHub/Axiom/middleware/requirements.txt)）：
```bash
pip install -r middleware/requirements.txt
```

### 步驟 2.3：編譯 C++ 核心並安裝至 Python 環境
使用 [setup.py](file:///Users/titus/Documents/GitHub/Axiom/middleware/setup.py) 將 C++ 核心代碼編譯並以可編輯模式（editable mode）安裝到您的虛擬環境中：
```bash
pip install -e middleware/
```
* **驗證編譯是否成功**：
  您可以執行以下指令測試是否能正常導入 C++ 模組：
  ```bash
  python -c "import pyaxiom; print('C++ 核心編譯成功！')"
  ```
  如果沒有報錯並印出成功訊息，代表編譯與安裝已完成。

---

## 3. 啟動網頁服務

本專案現已升級為 **Vue 3 + Vite + TypeScript** 架構，提供了兩種啟動與開發模式：

### 模式 A：本地開發模式 (前端熱重載 + 後端 API 代理)
此模式適用於前端功能開發，可享受 Vite 的 HMR 即時熱重載體驗。

1. **啟動後端 FastAPI 服務**：
   在專案根目錄啟用 Python 虛擬環境，並執行：
   ```bash
   uvicorn web.backend.main:app --reload
   ```
   後端將啟動於 **`http://127.0.0.1:8000`**。

2. **啟動前端 Vite 開發服務**：
   開啟另一個終端機視窗，進入 `web/frontend` 目錄安裝依賴並啟動：
   ```bash
   cd web/frontend
   npm install
   npm run dev
   ```
   前端開發伺服器將啟動於 **`http://localhost:5173`**。Vite 已配置代理 (Proxy) 自動將 `/api` 請求轉發至後端。

3. **打開瀏覽器**：
   訪問 👉 **[http://localhost:5173](http://localhost:5173)**。

---

### 模式 B：一體化部署模式 (編譯打包 + FastAPI 掛載 - 推薦)
此模式將前端程式碼編譯打包，直接由 FastAPI 作為靜態資源伺服器掛載，適合部署或不修改前端代碼時使用。

1. **前端打包編譯**：
   進入 `web/frontend` 目錄執行編譯打包：
   ```bash
   cd web/frontend
   npm install
   npm run build
   ```
   這將在 `web/frontend/dist/` 下生成編譯後的靜態檔案。

2. **啟動 FastAPI 服務**：
   在專案根目錄啟用 Python 虛擬環境，並啟動 FastAPI：
   ```bash
   uvicorn web.backend.main:app --reload
   ```
   FastAPI 會自動讀取並掛載 `web/frontend/dist` 的靜態網頁。

3. **打開瀏覽器**：
   訪問 👉 **[http://127.0.0.1:8000](http://127.0.0.1:8000)**。


---

## 4. 網頁功能與操作說明

進入網頁控制台後，您可以進行以下操作：

1. **參數設定**：
   * **選擇博弈模組**：目前支援「看牌百家樂 (Peek Baccarat)」。
   * **模擬局數**：從 10,000 局至 1,000,000,000 局（10 億局）。*(建議首次測試使用 100,000 局，僅需約 0.05 秒)*。
   * **主注投注方向**：選擇「閒家 (Player)」或「莊家 (Banker)」。
   * **隨機數種子 (Seed)**：設定不同的 RNG 種子以獲得不同的隨機序列。
2. **執行模擬**：
   * 點擊「**開始數值模擬**」按鈕，前端將發送請求至後端。
   * 後端將調用 C++ 核心進行快速蒙地卡羅模擬，接著利用 Python 中台分析數據，生成圖表（RTP 收斂圖、帳戶餘額走勢圖）與 Markdown 格式的分析報告。
3. **查看報告**：
   * 模擬完成後，網頁將即時渲染數值分析指標（RTP、標準差、最大回撤、莊/閒/和勝率分佈）。
   * 下方/右側將顯示自動生成的 Markdown 模擬分析報告與趨勢圖表。

---

## 5. 常見問題與排除 (Troubleshooting)

### Q1: 啟動後網頁顯示 `C++ core library (pyaxiom) is not compiled...`
* **原因**：尚未編譯安裝 `pyaxiom`，或者編譯後未安裝在目前的 Python 環境中。
* **解法**：請確保已在啟動虛擬環境的狀態下執行 `pip install -e middleware/`。

### Q2: 編譯時報錯 `error: Microsoft Visual C++ 14.0 or greater is required...` (Windows)
* **原因**：系統缺乏 C++ 編譯工具。
* **解法**：安裝 [Visual Studio 生成工具](https://visualstudio.microsoft.com/visual-cpp-build-tools/)，並確保選中「C++ Build Tools」。

### Q3: 編譯時提示 `pybind11` 缺失或無法解析
* **原因**：`pybind11` 尚未成功安裝在目前 Python 環境中。
* **解法**：請先執行 `pip install pybind11` 再重新編譯安裝。
