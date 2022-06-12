from distutils.core import setup, Extension

def main():
    setup(
        name="astar",
        version="6.9.6",
        ext_modules=[Extension("astar", sources=["src/priorityqueue.c", "src/utils.c", "astar.c"], include_dirs=["include"])]
    )

if __name__ == "__main__":
    main()
