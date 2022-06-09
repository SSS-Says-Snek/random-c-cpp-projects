from distutils.core import setup, Extension

def main():
    setup(
        name="test_thing",
        version="0.0.70",
        ext_modules=[Extension("test_thing", ["test.c"])]
    )

if __name__ == "__main__":
    main()
