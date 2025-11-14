#include "Song.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

// 初始化静态成员
int Song::next_id_ = 1;

// 匿名命名空间的辅助函数
namespace {
    std::string trim_copy(const std::string &s) {
        const std::string whitespace = " \t\n\r";
        size_t start = s.find_first_not_of(whitespace);
        if (start == std::string::npos)
            return "";
        size_t end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    }

    std::string to_lower_copy(const std::string &s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::string join_tags(const std::vector<std::string> &tags) {
        if (tags.empty())
            return "";
        std::ostringstream oss;
        for (size_t i = 0; i < tags.size() - 1; ++i) {
            oss << tags[i] << ", ";
        }
        oss << tags.back();
        return oss.str();
    }
}

// 构造函数实现
Song::Song(const std::string &title,
           const std::string &artist,
           int duration_sec,
           int rating)
{
    std::string t = trim_copy(title);
    std::string a = trim_copy(artist);

    bool ok = true;
    if (t.empty()) {
        std::cout << "[错误] 标题不能为空\n";
        ok = false;
    }
    if (a.empty()) {
        std::cout << "[错误] 艺人不能为空\n";
        ok = false;
    }
    if (duration_sec <= 0) {
        std::cout << "[错误] 时长必须为正整数（秒）\n";
        ok = false;
    }
    if (rating < 1 || rating > 5) {
        std::cout << "[错误] 评分必须在 1...5 之间\n";
        ok = false;
    }

    if (!ok) {
        valid_ = false;
        return;
    }

    id_ = next_id_++;
    title_ = std::move(t);
    artist_ = std::move(a);
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true;
}

// Setter 函数实现
bool Song::set_title(const std::string &t) {
    std::string tt = trim_copy(t);
    if (tt.empty()) {
        std::cout << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = std::move(tt);
    return true;
}

bool Song::set_artist(const std::string &a) {
    std::string aa = trim_copy(a);
    if (aa.empty()) {
        std::cout << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = std::move(aa);
    return true;
}

bool Song::set_duration(int sec) {
    if (sec <= 0) {
        std::cout << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        std::cout << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}

// --- 标签管理 ---
bool Song::add_tag(const std::string &tag) {
    std::string t = trim_copy(tag);
    if (t.empty()) {
        std::cout << "[提示] 空标签已忽略\n";
        return false;
    }
    std::string lower_t = to_lower_copy(t);
    for (const auto &existing : tags_) {
        if (to_lower_copy(existing) == lower_t) {
            std::cout << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }
    tags_.push_back(t);
    return true;
}


bool Song::remove_tag(const std::string &tag) {
    std::string t = trim_copy(tag);
    std::string lower_t = to_lower_copy(t);
    for (size_t i = 0; i < tags_.size(); ++i) {
        if (to_lower_copy(tags_[i]) == lower_t) {
            tags_.erase(tags_.begin() + static_cast<std::ptrdiff_t>(i));
            return true;
        }
    }
    std::cout << "[提示] 未找到该标签\n";
    return false;
}

// --- 功能函数 ---

bool Song::matches_keyword(const std::string &kw) const {
    std::string k = to_lower_copy(trim_copy(kw));
    if (k.empty())
        return false;

    if (to_lower_copy(title_).find(k) != std::string::npos)
        return true;
    if (to_lower_copy(artist_).find(k) != std::string::npos)
        return true;
    for (const auto &tg : tags_) {
        if (to_lower_copy(tg).find(k) != std::string::npos)
            return true;
    }
    return false;
}

// --- 友元重载 ---

std::ostream &operator<<(std::ostream &os, const Song &s) {
    os << "[#" << s.id_ << "] " << s.artist_ << " - " << s.title_ << " (" << s.duration_sec_ << "s) ";
    // 评分星号
    for (int i = 0; i < s.rating_; ++i)
        os << "*";
    if (!s.tags_.empty()) {
        os << "  [tags: " << join_tags(s.tags_) << "]";
    }
    return os;
}

bool operator<(const Song &a, const Song &b) {
    // rating 降序
    if (a.rating_ > b.rating_)
        return true;
    if (a.rating_ < b.rating_)
        return false;
    // title 升序
    if (a.title_ < b.title_)
        return true;
    if (a.title_ > b.title_)
        return false;
    // id 升序
    return a.id_ < b.id_;
}