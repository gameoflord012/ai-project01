#include "Config.h"
#include "Board.h"
#include "Helpers.h"

#include <cassert>

int BoardData::generate_random_target_index(unsigned int seed, int level, const set<int>& exclude_list)
{
    int random_index = -1;

    do
    {
        random_index = floor_index_list[level][seed % floor_index_list[level].size()];
        seed += 1;
    } while (exclude_list.count(random_index) > 0);

    return random_index;
}

void BoardData::printBoardData()
{
    printf("\n[Agent]\n");
    for (int e : agentIndexList) printf("%d ", e);
    printf("\n[Target]\n");
    for (int e : targetIndexList) printf("%d ", e);
    printf("\n[Key]\n");
    for (int e : keyIndexList) printf("%d ", e);
    printf("\n[Door]\n");
    for (int e : doorIndexList) printf("%d ", e);
}

Board::Board(int nRows, int nCols, int nHeight)
{
    dim.x = nRows;
    dim.y = nCols;
    dim.z = nHeight;

    gridData.resize(dim.x * dim.y * dim.z, -1);
}


BoardData Board::getBoardData() const {
    BoardData data;

    std::fill(data.agentIndexList, data.agentIndexList + MAX_AGENT_COUNT, -1);
    std::fill(data.targetIndexList, data.targetIndexList + MAX_AGENT_COUNT, -1);
    std::fill(data.keyIndexList, data.keyIndexList + MAX_KEY_COUNT, -1);
    std::fill(data.doorIndexList, data.doorIndexList + MAX_KEY_COUNT, -1);

    for (int i = 0; i < gridData.size(); i++)
    {
        int value = gridData[i];
        if ((value & 0xF) == AGENT)
        {
            int p = ROUND_INT(log2(value >> 4));
            data.agentIndexList[p] = i;
        }
        else if ((value & 0xF) == TARGET)
        {
            int p = ROUND_INT(log2(value >> 4));
            data.targetIndexList[p] = i;
        }
        else if ((value & 0xF) == KEY)
        {
            int p = ROUND_INT(log2(value >> 4));
            data.keyIndexList[p] = i;
        }
        else if ((value & 0xF) == DOOR)
        {
            int p = ROUND_INT(log2(value >> 4));
            data.doorIndexList[p] = i;
        }
        else if ((value & 0xF) == FLOOR)
        {
            data.floor_index_list[getPosition(i).z].push_back(i);
        }
    }

    for (auto e : data.floor_index_list)
    {
        if (RANDOM_SEED == -1)
            shuffle(e.begin(), e.end(),
                std::default_random_engine(generate_seed(gridData))
            );
    }

    return data;
}

int Board::getIndex(Position p) const
{
    if (p.x < 0 || p.x >= dim.x ||
        p.y < 0 || p.y >= dim.y ||
        p.z < 0 || p.z >= dim.z) return -1;

    return (p.z * dim.x + p.x) * dim.y + p.y;
}

Position Board::getPosition(int index) const
{
    return { index / dim.y % dim.x, index % dim.y, index / dim.y / dim.x };
}

void Board::setBoardData(Position p, int value)
{
    gridData[getIndex(p)] = value;
}

void Board::setBoardData(Position p, const char* value)
{
    gridData[getIndex(p)] = getValue(value);
}

int Board::getBoardValue(Position p) const
{
    return gridData[getIndex(p)];
}

int Board::getValue(const char* raw)
{
    if (STRING_EQUAL(raw, "-1"))        return OBSTACLE;
    else if (STRING_EQUAL(raw, "0"))    return FLOOR;
    else if (STRING_EQUAL(raw, "UP"))   return STAIR_UP;
    else if (STRING_EQUAL(raw, "DO"))   return STAIR_DOWN;
    else if (raw[0] == 'A') return GET_BOARD_VALUE(raw[1] - '1', AGENT);
    else if (raw[0] == 'T') return GET_BOARD_VALUE(raw[1] - '1', TARGET);
    else if (raw[0] == 'K') return GET_BOARD_VALUE(raw[1] - '1', KEY);
    else if (raw[0] == 'D') return GET_BOARD_VALUE(raw[1] - '1', DOOR);

    assert(0);
}

void Board::printBoard(int maxHeight)
{
    assert(maxHeight <= dim.z);

    NEW_PRINT_SECTION(BOARD INITIALIZATION)
        printf("\nGenerated board value: ");
    for (int k = 0; k < (maxHeight == -1 ? dim.z : maxHeight); k++)
    {
        printf("\nfloor %2d", k);
        for (int i = 0; i < dim.x; i++)
        {
            printf("\n");
            for (int j = 0; j < dim.y; j++)
            {
                printf("%3d|", gridData[getIndex({ i, j, k })]);
            }
        }
    }
}

bool Board::isTile(Position p, BoardTile c) const
{
    if (getIndex(p) < 0) return false;
    return (gridData[getIndex(p)] & 0xF) == c;
}

float Board::estimate_distance(int ia, int ib, bool is_euclidean) const
{
    assert(ia != -1 && ib != -1);

    if (is_euclidean)
        return getPosition(ia).distance(getPosition(ib));
    else
        return getPosition(ia).mahattan_distance(getPosition(ib));
}