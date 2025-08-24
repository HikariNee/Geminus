module;

export module TLS_Errors;

export enum class handshake_errors {
  RETRIES_EXHAUSTED,
  FATAL_ALERT
};
