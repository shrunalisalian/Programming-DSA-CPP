# include <iostream>
# include <vector>
using namespace std; 
// 121. Best Time to Buy and Sell Stock
int buySellStock(vector <int> &prices){
    int maxProfit = 0;
    int bestBuy = prices[0];

    for (int sell = 1; sell < prices.size(); sell++){
        if (prices[sell] > bestBuy){
            maxProfit = max(maxProfit, prices[sell] - bestBuy);
        }
        bestBuy = min(bestBuy, prices[sell]);
    }   
    return maxProfit;
}

int main(){
    vector <int> prices = {7,6,4,3,1};
    cout << "Max profit = " << buySellStock(prices)<< endl;
    return 0;
}