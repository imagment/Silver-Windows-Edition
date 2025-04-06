#include "Silver.hpp"

int main() {
    Actor c1;
    c1.AddComponent<Camera>();

    Actor actor("alert", "123\n456");
    
    actor.GetComponent<Transform>()->position = Vector3Zero;
    //actor.GetComponent<SpriteRenderer>()->alignShapeTo(1.0f);

    actor.AddObject();

    c1.GetComponent<Camera>()->RenderFrame();
    Hold();

    return 0;
}