import pandas as pd
import numpy as np

class SimulationAnalyzer:
    def __init__(self, raw_results):
        """
        傳入 C++ 模擬返回的原始結果列表
        """
        # 轉換為 pandas DataFrame
        self.df = pd.DataFrame([{
            'round_id': r.round_id,
            'bet_amount': r.bet_amount,
            'peek_fee': r.peek_fee,
            'raise_amount': r.raise_amount,
            'win_loss': r.win_loss,
            'game_status': r.game_status
        } for r in raw_results])
        
        # 預先計算總投注 (包含主注 + 手續費 + 加注)
        self.df['total_turnover'] = self.df['bet_amount'] + self.df['peek_fee'] + self.df['raise_amount']
        # 返還金額 = 總投入 + 淨損益
        self.df['payout'] = self.df['total_turnover'] + self.df['win_loss']

    def analyze_metrics(self) -> dict:
        """
        計算核心博弈指標
        """
        if self.df.empty:
            return {}

        total_rounds = len(self.df)
        total_bet = self.df['bet_amount'].sum()
        total_fee = self.df['peek_fee'].sum()
        total_raise = self.df['raise_amount'].sum()
        total_turnover = self.df['total_turnover'].sum()
        total_win_loss = self.df['win_loss'].sum()
        
        # RTP = 總返還 / 總投入
        rtp = (total_turnover + total_win_loss) / total_turnover if total_turnover > 0 else 0
        
        # 波動率 (以單局為單位)
        # 波動率 = 單局淨損益之標準差 / 單局主注金額 (通常以主注作為基準化)
        volatility = self.df['win_loss'].std() / 100.0 if total_rounds > 1 else 0
        
        # 最大回撤 (Max Drawdown)
        # 計算資金累計曲線
        cumulative_profit = self.df['win_loss'].cumsum()
        # 初始資金為 0，計算最高點
        running_max = cumulative_profit.cummax()
        # 回撤 = 最高點 - 當前點
        drawdown = running_max - cumulative_profit
        max_drawdown = drawdown.max()
        
        # 統計各局狀態佔比 (0=莊贏, 1=閒贏, 2=和)
        status_counts = self.df['game_status'].value_counts().to_dict()
        banker_wins = status_counts.get(0, 0)
        player_wins = status_counts.get(1, 0)
        ties = status_counts.get(2, 0)
        
        return {
            "total_rounds": int(total_rounds),
            "total_bet": float(total_bet),
            "total_fee": float(total_fee),
            "total_raise": float(total_raise),
            "total_turnover": float(total_turnover),
            "total_win_loss": float(total_win_loss),
            "rtp": float(rtp),
            "volatility": float(volatility),
            "max_drawdown": float(max_drawdown),
            "banker_win_rate": float(banker_wins / total_rounds) if total_rounds > 0 else 0,
            "player_win_rate": float(player_wins / total_rounds) if total_rounds > 0 else 0,
            "tie_rate": float(ties / total_rounds) if total_rounds > 0 else 0,
        }

    def get_trends(self, num_points=1000) -> dict:
        """
        取得降採樣後的收斂趨勢數據 (用於前端繪製圖表)
        """
        if self.df.empty:
            return {"rounds": [], "rtp": [], "balance": []}
            
        total_rounds = len(self.df)
        step = max(1, total_rounds // num_points)
        
        # 計算累計指標
        cum_win_loss = self.df['win_loss'].cumsum()
        cum_turnover = self.df['total_turnover'].cumsum()
        cum_rtp = (cum_turnover + cum_win_loss) / cum_turnover
        
        # 降採樣
        indices = np.arange(0, total_rounds, step)
        if indices[-1] != total_rounds - 1:
            indices = np.append(indices, total_rounds - 1)
            
        return {
            "rounds": self.df['round_id'].iloc[indices].tolist(),
            "rtp": cum_rtp.iloc[indices].tolist(),
            "balance": cum_win_loss.iloc[indices].tolist()
        }
