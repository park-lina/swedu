### Section 6: 객체를 생성하는 방법

```
class Rect : public Shape {
  Rect() {} // private 생성자
  public:
    static Shape* Create() { // 객체 생성 함수
      new Rect;
    }
};

int main() {
  Shape* p = Rect::Create();
}
```

객체 생성에 대한 제약을 만들 수 있다.

```
void CreateAndDraw(Shape* (*f)()) {
  Shape* p = f(); // 동적 생성
  p->Draw();
```

객체 생성 함수를 인자로 전달해서 (static이라 가능), 객체를 동적으로 생성할 수 있다.

1. Stack/Heap 객체 선언 (예: `Shape* p = new Rect;`)
2. Static 멤버 함수
3. 전용 클래스

객체의 생성을 분리 시키고, 객체 본체의 기능만 제공하게 된다.

```
class ShapeFactory {
  public:
    Shape* CreateShape(int type) {
      Shape* p = 0;
      switch(type) {
        case 1: p = new Rect; break; // friend 선언해서 가능
        case 2: p = new Circle; break;
      }
      return p;
    }
};

class Rect : public Shape {
  Rect() {}
  public:
    friend class ShapeFactory;
  }
};

int main() {
  ShapeFactory factory;
  Shape* p = factory.CreateShape(1);
```

