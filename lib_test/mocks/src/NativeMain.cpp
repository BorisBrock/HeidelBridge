// Trivial entry point so that `pio run -e native` links a (harmless) host
// binary. The native environment is test-only; this stub is excluded from
// test builds (where PlatformIO defines PIO_UNIT_TESTING) so the test suite's
// own main() is the only one linked. It is never compiled for the firmware
// environments, which do not depend on this library.
#ifndef PIO_UNIT_TESTING
int main()
{
    return 0;
}
#endif
