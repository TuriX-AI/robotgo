package robotgo

import (
	"sync"
	"testing"
)

func TestCheckKeyCodesBasic(t *testing.T) {
	keys := []string{"t", "a", "0", "9", "enter", "tab", "space"}
	for _, k := range keys {
		if _, err := checkKeyCodes(k); err != nil {
			t.Fatalf("checkKeyCodes(%q) returned error: %v", k, err)
		}
	}
}

func TestCheckKeyCodesConcurrentSingleChar(t *testing.T) {
	const workers = 16
	const loops = 200

	var wg sync.WaitGroup
	errCh := make(chan error, workers*loops)

	for i := 0; i < workers; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for j := 0; j < loops; j++ {
				if _, err := checkKeyCodes("t"); err != nil {
					errCh <- err
					return
				}
			}
		}()
	}

	wg.Wait()
	close(errCh)

	for err := range errCh {
		t.Fatalf("concurrent checkKeyCodes failed: %v", err)
	}
}
