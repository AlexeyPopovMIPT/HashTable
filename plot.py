import matplotlib.pyplot as pyplot
import json

with open ("table.json", "r") as file:
    data = json.loads(''.join(file.readlines()))


pyplot.bar(list(range(len(data["distribution"]))),
           data["distribution"], 
           width = 500 / (data["hashMaxValue"] + 1))
pyplot.title(data["comma"])
pyplot.xlabel('Значение хеша')
pyplot.ylabel('Частота встречаемости')
# pyplot.show()
pyplot.savefig("graph/"+data["comma"]+".png")


