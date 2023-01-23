#include "pyramid_schemes.h"

const std::array<PyramidScheme, NUM_AGES> pyramidSchemes = {{
    {{
        {true, {}},
        {true, {}},
        {false, {0}},
        {false, {0, 1}},
        {false, {1}},
        {true, {2}},
        {true, {2, 3}},
        {true, {3, 4}},
        {true, {4}},
        {false, {5}},
        {false, {5, 6}},
        {false, {6, 7}},
        {false, {7, 8}},
        {false, {8}},
        {true, {9}},
        {true, {9, 10}},
        {true, {10, 11}},
        {true, {11, 12}},
        {true, {12, 3}},
        {true, {13}}
    }},
    {{
        {true, {}},
        {true, {}},
        {true, {}},
        {true, {}},
        {true, {}},
        {true, {}},
        {false, {0, 1}},
        {false, {1, 2}},
        {false, {2, 3}},
        {false, {3, 4}},
        {false, {4, 5}},
        {true, {6, 7}},
        {true, {7, 8}},
        {true, {8, 9}},
        {true, {9, 10}},
        {false, {11, 12}},
        {false, {12, 13}},
        {false, {13, 14}},
        {true, {15, 16}},
        {true, {16, 17}}
    }},
    {{
        {true, {}},
        {true, {}},
        {false, {0}},
        {false, {0, 1}},
        {false, {1}},
        {true, {2}},
        {true, {2, 3}},
        {true, {3, 4}},
        {true, {4}},
        {false, {5, 6}},
        {false, {7, 8}},
        {true, {9}},
        {true, {9}},
        {true, {10}},
        {true, {10}},
        {false, {11, 12}},
        {false, {12, 13}},
        {false, {13, 14}},
        {true, {15, 16}},
        {true, {16, 17}}
    }}
}};
