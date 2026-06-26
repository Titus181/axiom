from typing import List

class SimulationResult:
    round_id: int
    bet_amount: float
    peek_fee: float
    raise_amount: float
    win_loss: float
    game_status: int
    detail: str

class SimulationSummary:
    total_rounds: int
    total_bet: float
    total_fee: float
    total_raise: float
    total_turnover: float
    total_win_loss: float
    rtp: float
    volatility: float
    max_drawdown: float
    trend_rounds: List[int]
    trend_rtps: List[float]
    trend_balances: List[float]
    banker_wins: int
    player_wins: int
    ties: int

class PeekBaccaratSimulator:
    def __init__(self, shoe_count: int = 8, default_bet: int = 1) -> None: ...
    def run_simulation(self, rounds: int) -> List[SimulationResult]: ...
    def run_simulation_summary(
        self, rounds: int, num_trend_points: int = 500
    ) -> SimulationSummary: ...
    def set_seed(self, seed: int) -> None: ...
    def set_bet_type(self, bet_type: int) -> None: ...
    def get_bet_type(self) -> int: ...
    def set_reveal_weights(self, weights: List[float]) -> None: ...
