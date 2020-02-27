import csv

csv_dir = './data20.csv'
with open(csv_dir) as csvfile:
      readCSV = csv.reader(csvfile, delimiter = ',')
      for row in readCSV:
        l = row[0]
        text = row[1].replace("<br />", " ")
        print('{}:{}'.format(l, text))
        print(' ')
        # if l == '1':
        #   l = 'pos'
        # elif l == '0':
        #   l = 'neg'
        # else:
        #   continue
        # examples.append(InputExample(
        #     guid="unused_id", text_a=text, text_b=None, label=l))