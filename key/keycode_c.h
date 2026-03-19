#include "keycode.h"

#if defined(IS_MACOSX)
	#include <CoreFoundation/CoreFoundation.h>
	#include <Carbon/Carbon.h> /* For kVK_ constants, and TIS functions. */
	#include <pthread.h>
	#include <stdint.h>

	/* Returns string representation of key, if it is printable. 
	Ownership follows the Create Rule; 
	it is the caller's responsibility to release the returned object. */
	CFStringRef createStringForKey(CGKeyCode keyCode);

	/* keyCodeForChar cache and lock (process-wide, thread-safe). */
	static CFMutableDictionaryRef charToCodeDict = NULL;
	static pthread_mutex_t charToCodeDictMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

MMKeyCode keyCodeForChar(const char c) {
	#if defined(IS_MACOSX)
		/* OS X does not appear to have a built-in function for this, 
		so instead it to write our own. */
		CGKeyCode code;
		UniChar character = (unsigned char)c;
		CFStringRef charStr = NULL;

		pthread_mutex_lock(&charToCodeDictMutex);

		/* Generate table of keycodes and characters. */
		if (charToCodeDict == NULL) {
			size_t i;
			charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 128,
				&kCFCopyStringDictionaryKeyCallBacks, NULL);
			if (charToCodeDict == NULL) {
				pthread_mutex_unlock(&charToCodeDictMutex);
				return K_NOT_A_KEY;
			}

			/* Loop through every keycode (0 - 127) to find its current mapping. */
			for (i = 0; i < 128; ++i) {
				CFStringRef string = createStringForKey((CGKeyCode)i);
				if (string != NULL) {
					/* CFDictionary does not allow NULL value; store keycode+1. */
					uintptr_t encodedCode = (uintptr_t)i + 1;
					CFDictionaryAddValue(charToCodeDict, string, (const void *)encodedCode);
					CFRelease(string);
				}
			}
		}

		charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &character, 1);
		if (charStr == NULL) {
			pthread_mutex_unlock(&charToCodeDictMutex);
			return K_NOT_A_KEY;
		}
		/* Our values may be NULL (0), so we need to use this function. */
		/* Use pointer-sized variable to avoid stack overflow on 64-bit systems */
		const void *codePtr = NULL;
		if (!CFDictionaryGetValueIfPresent(charToCodeDict, charStr, &codePtr)) {
			code = UINT16_MAX; /* Error */
		} else {
			uintptr_t encodedCode = (uintptr_t)codePtr;
			if (encodedCode == 0) {
				code = UINT16_MAX;
			} else {
				code = (CGKeyCode)(encodedCode - 1);
			}
		}
		CFRelease(charStr);
		pthread_mutex_unlock(&charToCodeDictMutex);

		// TISGetInputSourceProperty may return nil so we need fallback
		if (code == UINT16_MAX) {
			return K_NOT_A_KEY;
		}

		return (MMKeyCode)code;
	#elif defined(IS_WINDOWS)
		MMKeyCode code;
		code = VkKeyScan(c);
		if (code == 0xFFFF) {
			return K_NOT_A_KEY;
		}

		return code;
	#elif defined(USE_X11)
		char buf[2];
		buf[0] = c;
		buf[1] = '\0';

		MMKeyCode code = XStringToKeysym(buf);
		if (code == NoSymbol) {
			/* Some special keys are apparently not handled properly */
			struct XSpecialCharacterMapping* xs = XSpecialCharacterTable;
			while (xs->name) {
				if (c == xs->name) {
					code = xs->code;
					// 
					break;
				}
				xs++;
			}
		}

		if (code == NoSymbol) {
			return K_NOT_A_KEY;
		}

		// x11 key bug
		if (c == 60) {
			code = 44;
		}
		return code;
	#endif
}

#if defined(IS_MACOSX)
	CFStringRef createStringForKey(CGKeyCode keyCode){
		// TISInputSourceRef currentKeyboard = TISCopyCurrentASCIICapableKeyboardInputSource();
		TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardLayoutInputSource();

		/* Check if currentKeyboard is NULL to avoid crash */
		if (currentKeyboard == NULL) { return NULL; }

		CFDataRef layoutData = (CFDataRef) TISGetInputSourceProperty(
			currentKeyboard, kTISPropertyUnicodeKeyLayoutData);

		if (layoutData == nil) {
			CFRelease(currentKeyboard);  /* Fix memory leak */
			return NULL;
		}

		const UCKeyboardLayout *keyboardLayout = (const UCKeyboardLayout *) CFDataGetBytePtr(layoutData);
		UInt32 keysDown = 0;
		UniChar chars[4];
		UniCharCount realLength;

		UCKeyTranslate(keyboardLayout, keyCode, kUCKeyActionDisplay, 0, LMGetKbdType(),
					kUCKeyTranslateNoDeadKeysBit, &keysDown,
					sizeof(chars) / sizeof(chars[0]), &realLength, chars);
		CFRelease(currentKeyboard);
		if (realLength == 0) {
			return NULL;
		}

		return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
	}
#endif
