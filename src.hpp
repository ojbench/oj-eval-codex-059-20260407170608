#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

class BaseJudger {
public:
    BaseJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : time_limit_(time_limit), memory_limit_(memory_limit), score_(0) {
        if (answer) {
            size_t len = std::strlen(answer);
            answer_ = new char[len + 1];
            std::memcpy(answer_, answer, len + 1);
        } else {
            answer_ = new char[1];
            answer_[0] = '\0';
        }
    }

    virtual void Submit(size_t time, size_t memory, const char *output) = 0;

    size_t GetScore() const { return score_; }

    virtual ~BaseJudger() {
        delete[] answer_;
        answer_ = nullptr;
    };

protected:
    char *answer_;
    const size_t time_limit_;
    const size_t memory_limit_;
    size_t score_;

    virtual bool CheckAnswer(const char *output) const {
        // the output must equal to the answer
        return std::strcmp(answer_, output) == 0;
    }
};


class OIJudger : public BaseJudger {
public:
    OIJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer), submitted_(false) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        if (submitted_) {
            // Multiple submissions cause zero score
            score_ = 0;
            return;
        }
        submitted_ = true;
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            score_ = 100;
        } else {
            score_ = 0;
        }
    }

protected:
    bool CheckAnswer(const char *output) const override {
        // Compare ignoring trailing spaces at end of each line
        return NormalizeAndCompare(answer_, output);
    }

private:
    bool submitted_;

    static bool NormalizeAndCompare(const char *a, const char *b) {
        // Process both strings line by line, trimming trailing spaces ' ' from each line
        // and compare lines exactly.
        const char *pa = a;
        const char *pb = b;
        while (true) {
            std::string la, lb;
            bool enda = !ReadLine(pa, la);
            bool endb = !ReadLine(pb, lb);
            // Trim trailing spaces
            RTrimSpaces(la);
            RTrimSpaces(lb);
            if (la != lb) return false;
            if (enda && endb) return true;
            if (enda != endb) return false;
        }
    }

    static bool ReadLine(const char *&p, std::string &out) {
        if (*p == '\0') return false;
        out.clear();
        while (*p != '\0' && *p != '\n') {
            out.push_back(*p);
            ++p;
        }
        if (*p == '\n') {
            // consume newline
            ++p;
        }
        return true;
    }

    static void RTrimSpaces(std::string &s) {
        while (!s.empty() && s.back() == ' ') s.pop_back();
    }
};


class ICPCJudger : public BaseJudger {
public:
    ICPCJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        size_t cur = 0;
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            cur = 100;
        }
        if (cur > score_) score_ = cur;
    }
};


class SpacialJudger : public BaseJudger {
public:
    SpacialJudger(size_t time_limit, size_t memory_limit,
                  size_t full_score_time, size_t full_score_memory,
                  const char *answer)
        : BaseJudger(time_limit, memory_limit, answer),
          fst_(full_score_time), fsm_(full_score_memory) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        size_t cur = 0;
        if (CheckAnswer(output)) {
            int ts = CalcScore(time, fst_, time_limit_);
            int ms = CalcScore(memory, fsm_, memory_limit_);
            // final score = floor(ts * ms / 100)
            if (ts < 0) ts = 0; if (ms < 0) ms = 0;
            cur = static_cast<size_t>((static_cast<long long>(ts) * ms) / 100);
        }
        if (cur > score_) score_ = cur;
    }

private:
    size_t fst_;
    size_t fsm_;

    static int CalcScore(size_t value, size_t full_score_value, size_t limit_value) {
        if (value >= limit_value) return 0;
        if (value <= full_score_value) return 100;
        if (full_score_value >= limit_value) {
            // Degenerate: everything below limit gets full score
            return 100;
        }
        // Linear from 100 at full_score_value to 0 at limit_value
        size_t denom = (limit_value - full_score_value);
        size_t numer = (limit_value - value);
        // floor(100 * numer / denom)
        return static_cast<int>((100ULL * numer) / denom);
    }
};

