#include <iostream>
#include <expected>

std::expected<int, std::string> safe_divide(int numerator, int denominator) {
    if (denominator == 0) {
        return std::unexpected("Error: Division by zero");
    }
    return numerator / denominator;
}

struct Point {
    int x,y;
    void move(this Point& self, int dx, int dy) {
        self.x = dx;
        self.y = dy;
    }

    /*const Point& reflect() const {
        return *this;
    };

    Point& reflect() {
        x = -x; y = -y;
        return *this;
    }*/

    auto reflect(this auto& self) {
        if constexpr (std::is_const_v< std::remove_reference_t<decltype(self)>>) {
            std::cout << "reflect const\n";
        }
        else {
            std::cout << "reflect non-const\n";
            self.x = -self.x; self.y = -self.y;
        }
        return self;
    }
    void display(this const Point& self) {
        std::cout << "Point(" << self.x << ", " << self.y << ")\n";
    }
};

int main() {
    auto result = safe_divide(10, 4);
    if(result) {
        std::cout << "Result : " << result.value() << '\n';
    }else {
        std::cout << result.error() << '\n';
    }
    Point p{1, 2};
    p.move(3, 4);
    const Point cp{3,4};
    p.reflect().display();
    cp.reflect().display();



    return 0;
}

