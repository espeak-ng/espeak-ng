
import os
import pandas as pd



def transcript(src_text):
    cmd = f'../src/espeak-ng "{src_text}" --ipa -v ca -q'
    output = str(os.popen(cmd).read())[:-1]
    return output

data = pd.read_csv('data/tests.tsv', sep="\t")


data["transcripcio_espeak"] = data.apply(lambda x: transcript(x["segment"]), axis=1)
data["correccio"] = data["transcripció"] == data["transcripcio_espeak"]
print(data[data['transcripció'].notnull()][data['correccio'] == False][["segment", "transcripció", "transcripcio_espeak"]])

data.to_csv('data/results_test.tsv', sep="\t")
