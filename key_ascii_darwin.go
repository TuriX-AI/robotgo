//go:build darwin

package robotgo

func asciiKeyCode(k byte) (int, bool) {
	// Normalize ASCII upper-case letters.
	if k >= 'A' && k <= 'Z' {
		k = k - 'A' + 'a'
	}

	// macOS virtual keycodes for ANSI layout.
	// Ref: HIToolbox/Events.h (kVK_ANSI_*)
	switch k {
	case 'a':
		return 0x00, true
	case 's':
		return 0x01, true
	case 'd':
		return 0x02, true
	case 'f':
		return 0x03, true
	case 'h':
		return 0x04, true
	case 'g':
		return 0x05, true
	case 'z':
		return 0x06, true
	case 'x':
		return 0x07, true
	case 'c':
		return 0x08, true
	case 'v':
		return 0x09, true
	case 'b':
		return 0x0B, true
	case 'q':
		return 0x0C, true
	case 'w':
		return 0x0D, true
	case 'e':
		return 0x0E, true
	case 'r':
		return 0x0F, true
	case 'y':
		return 0x10, true
	case 't':
		return 0x11, true
	case '1':
		return 0x12, true
	case '2':
		return 0x13, true
	case '3':
		return 0x14, true
	case '4':
		return 0x15, true
	case '6':
		return 0x16, true
	case '5':
		return 0x17, true
	case '9':
		return 0x19, true
	case '7':
		return 0x1A, true
	case '8':
		return 0x1C, true
	case '0':
		return 0x1D, true
	case 'o':
		return 0x1F, true
	case 'u':
		return 0x20, true
	case 'i':
		return 0x22, true
	case 'p':
		return 0x23, true
	case 'l':
		return 0x25, true
	case 'j':
		return 0x26, true
	case 'k':
		return 0x28, true
	case 'n':
		return 0x2D, true
	case 'm':
		return 0x2E, true
	default:
		return 0, false
	}
}
