---
title: "Project Axiom: 專案文件總覽與 Obsidian 導航指南"
version: v1.0.0
date: 2026-06-25
author: Axiom Development Team
tags:
  - axiom
  - documentation
  - navigation
  - obsidian-friendly
---

# Project Axiom: 專案文件總覽與 Obsidian 導航指南

> [!NOTE]
> 本文件為 **Project Axiom** 的文件導航總覽（README）。旨在為產品經理 (PM)、數值策劃與開發團隊提供清晰的文件庫結構，並完美相容 **Obsidian** 雙向連結與屬性（Properties）檢索。

---

## 📂 文件目錄結構與地圖

我們已將專案中所有重要文件進行分門別類的整理，結構如下：

```text
docs/
├── README.md                           # 本導覽文件 (Obsidian 導航起點)
├── game_rules/
│   └── peek_baccarat_rules.md          # 看牌百家樂規則與設計說明書
├── game_math/
│   └── volatility_guide.md             # 博弈數值波動率與風險控制設計指南
└── archive/
    └── original_spec_v2.docx           # 原始需求與開發規格 V2 封存檔
```

---

## 🔗 Obsidian 快速導航連結

您可以直接在 Obsidian 中點擊以下**雙向連結 (Wikilinks)** 跳轉至對應文件：

### 🎮 1. 遊戲玩法與規則設計
* 📄 **[[game_rules/peek_baccarat_rules|看牌百家樂規則與設計說明書]]**
  * *涵蓋內容*：看牌機制、手續費模式、AI 最優加注決策策略、結算賠率表、10 億局真實模擬收斂報告 (RTP: `98.8033%`)。

### 📊 2. 數值數學與營運風控
* 📄 **[[game_math/volatility_guide|博弈數值波動率與風險控制設計指南]]**
  * *涵蓋內容*：波動率的數學公式與計算方式、看牌百家樂波動率（`2.0160`）偏高的原因剖析、RTP 收斂速度預測模型（95% 信賴區間）、最大資金回撤 (MDD) 與賭場儲備金安全警示。

### 🗃️ 3. 原始規格封存
* 📂 **[原始需求規格說明書 V2](file:///./archive/original_spec_v2.docx)**
  * *說明*：專案啟動時的原始 Word 規格設計文件，已移至封存資料夾（`archive/`）以利後續版本追溯。

---

## 💡 文件使用建議

1. **對於產品經理 (PM)**：
   * 建議優先閱讀 **[[game_rules/peek_baccarat_rules|看牌百家樂規則與設計說明書]]**，以了解特有的強制看牌手續費與隨機揭牌加注規則。
2. **對於數值企劃與營運風控**：
   * 建議深入閱讀 **[[game_math/volatility_guide|博弈數值波動率與風險控制設計指南]]**，其中包含了賭場在上線該款中高波動率遊戲時，所需的儲備金控制與單日損益波動預期。
3. **對於開發人員**：
   * 本文件庫所引用的所有變數（如 `rtp`、`volatility`、`max_drawdown`）皆與 C++ 模擬核心、Python 數據中台無縫命名對齊，可直接作爲數值驗證的程式碼開發參考。
