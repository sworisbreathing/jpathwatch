import static ch.qos.logback.classic.Level.*
import ch.qos.logback.classic.encoder.PatternLayoutEncoder
import ch.qos.logback.core.ConsoleAppender
appender("STDOUT", ConsoleAppender) {
  encoder(PatternLayoutEncoder) {
    pattern = "%d{yyyy-MM-dd HH:mm:ss.SSS} [%c{5} %L] %p - %msg%n"
  }
}
root(WARN, ["STDOUT"])
logger("name.pachler",ALL)
logger("jpathwatch",ALL)