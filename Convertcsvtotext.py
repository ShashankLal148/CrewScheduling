import csv

inp_file = "train_edges_repeated_days.csv"
out_file = "train_edges.txt"

with open(inp_file, "r") as csv_file, open(out_file, "w") as txt_file:
    csv_reader = csv.reader(csv_file)
    for row in csv_reader:
        txt_file.write(",".join(row) + "\n")

print("CSV converted to text successfully!")
