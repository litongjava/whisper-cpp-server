
COPY models/ggml-large-v3.bin /app/models/

EXPOSE 8080

CMD ["/app/whisper_http_server_base_httplib", "-m", "/app/models/ggml-base.en.bin"]