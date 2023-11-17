#include <GLFW/glfw3.h>

int main(void) {
  GLFWwindow *window;

  // GLFWライブラリの初期化
  if (!glfwInit())
    return -1;

  // ウィンドウの作成
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // 作成したウィンドウを現在のコンテキストに設定
  glfwMakeContextCurrent(window);

  // ウィンドウが閉じられるまでループ

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0) {
    // レンダリングここから

    // 背景色を設定
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // レンダリングここまで

    // フロントバッファとバックバッファを交換
    glfwSwapBuffers(window);

    // イベントをポーリング
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
