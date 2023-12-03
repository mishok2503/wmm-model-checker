#include "tokenizer.h"

#include <unordered_map>

namespace {
tokens::Token TokenFromString(const std::string& str) {
    static const std::unordered_map<std::string, tokens::Token> kMap = {
        {"+", tokens::BinaryOperator{BinaryOperator::PLUS}},
        {"-", tokens::BinaryOperator{BinaryOperator::MINUS}},
        {"*", tokens::BinaryOperator{BinaryOperator::MULTIPLY}},
        {"/", tokens::BinaryOperator{BinaryOperator::DIVIDE}},
        {"RLX", tokens::MemoryModel{MemoryModel::RLX}},
        {"REL", tokens::MemoryModel{MemoryModel::REL}},
        {"ACQ", tokens::MemoryModel{MemoryModel::ACQ}},
        {"REL_ACQ", tokens::MemoryModel{MemoryModel::REL_ACQ}},
        {"SEQ_CST", tokens::MemoryModel{MemoryModel::SEQ_CST}},
        {"=", tokens::Assigment{}},
        {":=", tokens::ReturnAssigment{}},
        {"if", tokens::If{}},
        {"goto", tokens::Goto{{""}}},
        {"load", tokens::Load{}},
        {"store", tokens::Store{}},
        {"fence", tokens::Fence{}},
        {"cas", tokens::Cas{}},
        {"fai", tokens::Fai{}}};
    if (kMap.contains(str)) {
        return kMap.at(str);
    }
    if (str.front() == '#') {
        return tokens::MemoryAt{std::get<tokens::Register>(TokenFromString({str.begin() + 1, str.end()}))};
    }
    if (str.front() == 'r') {
        int num = std::atoi(str.c_str() + 1);
        if (num < 0 || num >= 16) {
            throw SyntaxError{"Wrong register name"};
        }
        return tokens::Register{static_cast<Register>(num)};
    }
    if (std::isdigit(str.front()) || str.front() == '-') {
        size_t pos;
        int value = std::stoi(str, &pos);
        if (pos != str.size()) {
            throw SyntaxError{"Bad number"};
        }
        return tokens::Constant{value};
    }
    throw SyntaxError{"Unknown token"};
}

}  // namespace

void Tokenizer::Next() {
    line.clear();
    std::string str;
    while (getline(in, str)) {
        if (std::all_of(str.begin(), str.end(), [](char c) { return std::isspace(c); })) {
            continue;
        }
        std::stringstream ss{str};
        ss >> str;
        if (str.back() == ':') {
            str.pop_back();
            line.push_back(tokens::Label{std::move(str)});
        } else {
            ss.seekg(0);
        }
        for (int i = 0; i < kMaxTokensPerLine; ++i) {
            if (!(ss >> str)) {
                return;
            }
            auto token = TokenFromString(str);
            if (auto* gt = std::get_if<tokens::Goto>(&token)) {
                if (!(ss >> gt->label.name)) {
                    throw SyntaxError{"No label after \"goto\""};
                }
            }
            line.push_back(std::move(token));
        }
        throw SyntaxError{"Line can't contain more then " + std::to_string(kMaxTokensPerLine) + " tokens"};
    }
    is_end = true;
}