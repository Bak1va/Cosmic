#include "Game.hpp"
#include <mutex>
#include <vector>
#include <algorithm>

namespace logic {

class DummyGame : public IGame {
public:
    DummyGame() = default;
    ~DummyGame() override = default;

    void add_observer(IObserver* obs) override {
        if (!obs) return;
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.push_back(obs);
    }

    void remove_observer(IObserver* obs) override {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.erase(std::remove(observers_.begin(), observers_.end(), obs), observers_.end());
    }

    void start() override { set_state(GameState::Running); }
    void pause() override { set_state(GameState::Paused); }
    void finish() override { set_state(GameState::Finished); }

    GameState state() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_;
    }

private:
    void set_state(GameState s) {
        std::vector<IObserver*> copy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            state_ = s;
            copy = observers_;
        }
        for (auto obs : copy) {
            if (obs) obs->on_state_changed(s);
        }
    }

    mutable std::mutex mutex_;
    std::vector<IObserver*> observers_;
    GameState state_ = GameState::NotStarted;
};

std::unique_ptr<IGame> create_game() {
    return std::make_unique<DummyGame>();
}

} // namespace logic
