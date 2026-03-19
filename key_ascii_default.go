//go:build !darwin

package robotgo

func asciiKeyCode(_ byte) (int, bool) {
	return 0, false
}
