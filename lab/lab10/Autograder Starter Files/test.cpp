#include "deals.h"
#include <iostream>
#include <vector>
#include <map>
#include <functional>

using cost = long long;

// Project Identifier: D7E20F91029D0CB08715A2C54A782E0E8DF829BF

/****************************************************
 *                                                  *
 *  (\     ___        Congrats on making it to      *
 *  (  \  /(o)\       the last lab of EECS 281!     *
 *  (   \/  ()/ /)    I hope you enjoyed the ride.  *
 *   (   `;.))'".)                                  *
 *    `(/////.-'      Good luck, and have fun       *
 * =====))=))===()    with dynamic programming!     *
 *   ///'                                           *
 *  //   PjP/ejm                ~ EECS 281 Staff    *
 * '                                                *
 *                                                  *
 ****************************************************/

int main() {
    // sample #5
    // # meals == 9
    std::vector<cost> sample_input = {1000, 2, 2, 2, 1001, 1002, 1003, 1004, 1005};
    std::cout << "[meal  #5] ... " << std::flush;
    cost student_answer = best_price(sample_input);
    if (student_answer != 4263) {
        std::cout << "FAIL: expected 4263 but got " << student_answer << std::endl;
        // for your reference in debugging the above test case:
        // total cost with no discount or promotion: 6021
        // total cost if only discounting with coupons: 4512
        // total cost if always applying punchcard: 5019
    } else {
        std::cout << "PASS" << std::endl;
    }
}