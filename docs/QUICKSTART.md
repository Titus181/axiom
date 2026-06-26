# Project Axiom 快速啟動指南

如果您已經完成 C++ 核心的編譯與安裝，請執行以下指令啟動網頁服務：

## 🚀 推薦模式：一體化部署模式

此模式最簡單，無需前端開發伺服器，直接由 FastAPI 服務前後端網頁：

```bash
# 1. 進入前端目錄並完成編譯打包 (若已編譯過可跳過)
cd web/frontend
npm run build
cd ../..

# 2. 啟用 Python 虛擬環境
source .venv/bin/activate

# 3. 啟動網頁服務
uvicorn web.backend.main:app --reload
```

啟動後，請在瀏覽器中打開：
👉 **[http://127.0.0.1:8000](http://127.0.0.1:8000)**

---

## 🛠️ 開發模式 (前端熱更新)

如果您需要開發前端功能，請同時啟動前後端服務：

1. **啟動後端 API**：
   ```bash
   source .venv/bin/activate
   uvicorn web.backend.main:app --reload
   ```
2. **啟動前端 Vite** (在另一個終端機視窗)：
   ```bash
   cd web/frontend
   npm run dev
   ```
3. 訪問：👉 **[http://localhost:5173](http://localhost:5173)**
