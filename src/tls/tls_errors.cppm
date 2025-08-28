module;

export module TLS_Errors;

export enum class handshake_errors {
  RETRIES_EXHAUSTED,
  FATAL_ALERT
};

export enum class transport_errors {
  FATAL_ALERT,
  NON_FATAL_ALERT
};
