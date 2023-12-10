#%%
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import argparse

# Read file on passed arguments
parser = argparse.ArgumentParser()
parser.add_argument("-heat", help="path to heatmap file")
args = parser.parse_args()

heat_path = args.heat
# map_path = 'input/input1-level3.txt'
# heat_path = 'heatmap.txt'


# Get filename
heat_filename = heat_path.split('\\')[-1]

def read_board(file_path):
    boards = []
    with open(file_path, 'r') as file:
        row, col = map(int, file.readline().split(','))
        current_floor_board = None

        for line in file.readlines():
            if line.startswith('[floor') or line.startswith('[Floor'):
                floor = int(line[6])
                if current_floor_board is not None:
                    boards.append(current_floor_board)
                    current_floor_board = []
                else:
                    current_floor_board = []
            elif line:
                current_floor_board.append(list(line.strip().split(',')))
        boards.append(current_floor_board)
    return boards

def truncate_map(boardMap):
    i = 0
    for floor in range(len(boardMap)):
        floor_sum = 0
        for row in boardMap[floor]:
            floor_sum += sum(map(float, row))
        if floor_sum == 0:
            i = floor
            break
    boardMap = boardMap[:i]
    return boardMap
            
heatMap = read_board(heat_path)
heatMap = truncate_map(heatMap)


for i in range(len(heatMap)):
    tmp = np.array(heatMap[i]).astype(int)
    fig, ax = plt.subplots(figsize=(20,20)) 
    graph = sns.heatmap(tmp, annot=True, cmap='Blues')
    graph.set_title('Heatmap of floor {}'.format(i+1))
    plt.close()

    # Export to png
    fig = graph.get_figure()
    fig.savefig('heatmap_floor{}.png'.format(i+1))
    print('Heatmap of floor {} is exported to heatmap_floor{}.png'.format(i+1, i+1))