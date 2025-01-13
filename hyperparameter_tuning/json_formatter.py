import pandas as pd
import json
import os
from pandas import json_normalize

def format_json(json_file:str)-> pd.DataFrame: 
    
    with open(json_file, "r") as file:
        data = json.load(file)

    df = json_normalize(data, sep='_')
    file_path =  os.path.dirname(json_file)
    csv_file = os.path.join(file_path, 'results.csv')
    df.to_csv(csv_file, index=False)

    return df
    



if __name__ == '__main__':
    json_file = "/Users/juliusmiers/KidneyExchangeOptimization/hyperparameter_tuning/results.json"
    df = format_json(json_file)