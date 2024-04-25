FROM litongjava/whisper-cpp-server:1.0.0

COPY models/ggml-tiny.en-q5_1.bin /app/models/

EXPOSE 8080

CMD ["/app/whisper_http_server_base_httplib", "-m", "/app/models/ggml-tiny.en-q5_1.bin"]