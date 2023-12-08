#include "SearchState.h"
#include "Search.h"
#include "Helpers.h"

int main()
{
    SearchResultData resultData;
    shared_ptr<Board> board;

    algorithm::read(board, "../input/inp.txt");
    bool isSearchSuccess = algorithm::search(board, resultData);

    NEW_PRINT_SECTION(RESULT)
    if (isSearchSuccess)
    {
        printf("\npath go:");
        resultData.print_path();

        resultData.printResult();
    }
    else
    {
        printf("\nno solution found");
    }
}