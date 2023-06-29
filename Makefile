all:
	@cd keyboard_test/ && make && mv keyboard_test.ko ../
	@cd video_test/ && make && mv video_test.ko ../
	
clean:
	@rm keyboard_test.ko && cd keyboard_test/ && make clean
	@rm video_test.ko && cd video_test/ && make clean
	
keyboard:
	@cd ClientApp/ && python3 client_keyboard.py

video:
	@cd ClientApp/ && python3 client_video.py

