#!/usr/bin/env python3

import argparse
import requests

def get_bitcoin_price():
    """Gets the current Bitcoin price in USD from the CoinGecko API."""
    try:
        response = requests.get("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd")
        response.raise_for_status()
        data = response.json()
        return float(data["bitcoin"]["usd"])
    except requests.exceptions.RequestException as e:
        print(f"Error getting Bitcoin price: {e}")
        return None

def get_block_height():
    """Gets the current block height from the blockchain.info API."""
    try:
        response = requests.get("https://blockchain.info/q/getblockcount")
        response.raise_for_status()
        return int(response.text)
    except requests.exceptions.RequestException as e:
        print(f"Error getting block height: {e}")
        return None

def get_difficulty():
    """Gets the current network difficulty from the blockchain.info API."""
    try:
        response = requests.get("https://blockchain.info/q/getdifficulty")
        response.raise_for_status()
        return float(response.text)
    except requests.exceptions.RequestException as e:
        print(f"Error getting network difficulty: {e}")
        return None

def calculate_block_reward(height):
    """Calculates the current block reward based on the block height."""
    if height is None:
        return None
    # Block reward halves every 210,000 blocks
    halvings = height // 210000
    if halvings == 0:
        return 50
    else:
        return 50 / (2 ** halvings)

def main():
    """
    Calculates the ROI of Bitcoin mining.

    Example usage:
    python3 roi_calculator.py --hashrate 110 --power 3250 --cost 0.1 --pool_fee 1
    """
    parser = argparse.ArgumentParser(description="Calculate Bitcoin mining ROI.")
    parser.add_argument("--hashrate", type=float, required=True, help="Your hashrate in TH/s.")
    parser.add_argument("--power", type=float, required=True, help="Your power consumption in Watts.")
    parser.add_argument("--cost", type=float, required=True, help="Your electricity cost in USD per kWh.")
    parser.add_argument("--pool_fee", type=float, default=0.0, help="Your mining pool fee in percent.")
    args = parser.parse_args()

    price = get_bitcoin_price()
    height = get_block_height()
    difficulty = get_difficulty()
    reward = calculate_block_reward(height)

    if price is None or reward is None or difficulty is None:
        return

    # Seconds in a day
    seconds_in_a_day = 24 * 60 * 60

    # Calculate daily revenue
    # This is a simplified calculation and does not account for network hashrate fluctuations.
    daily_revenue = (args.hashrate * 1e12 / (difficulty * 2**32)) * reward * seconds_in_a_day * price * (1 - args.pool_fee / 100)

    # Calculate daily cost
    daily_cost = (args.power / 1000) * 24 * args.cost

    # Calculate net profit and ROI
    net_profit = daily_revenue - daily_cost
    roi = (net_profit / daily_cost) * 100 if daily_cost > 0 else float('inf')

    print("--- Bitcoin Mining ROI Calculator ---")
    print(f"Current Bitcoin Price: ${price:,.2f}")
    print(f"Current Block Reward: {reward} BTC")
    print(f"Current Network Difficulty: {difficulty:,.2f}")
    print("------------------------------------")
    print(f"Daily Revenue: ${daily_revenue:,.2f}")
    print(f"Daily Cost: ${daily_cost:,.2f}")
    print("------------------------------------")
    print(f"Net Profit: ${net_profit:,.2f}")
    print(f"ROI: {roi:.2f}%")
    print("------------------------------------")

    if net_profit > daily_cost * 0.1:
        print("✅ Your net profit is greater than 10% of your costs.")
    else:
        print("❌ Your net profit is not greater than 10% of your costs.")

if __name__ == "__main__":
    main()
