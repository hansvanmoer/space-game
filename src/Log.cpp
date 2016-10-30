#include "Log.h"

using namespace Game;
using namespace Game::Log;

using namespace std;

Output::Output(LoggerFactory &factory, const Id &id) : factory_(factory), id_(id), level_(Level::NONE) {
    ostream *ignore = factory_.ignore();
    debug_output_ = ignore;
    info_output_ = ignore;
    warning_output_ = ignore;
    error_output_ = ignore;
    fatal_output_ = ignore;
};

void Output::configure(Level level, std::ostream *stream) {
    ostream *ignore = factory_.ignore();
    level_ = level;
    switch (level_) {
        case Level::DEBUG:
            debug_output_ = stream;
            info_output_ = stream;
            warning_output_ = stream;
            error_output_ = stream;
            fatal_output_ = stream;
            break;
        case Level::INFO:
            debug_output_ = ignore;
            info_output_ = stream;
            warning_output_ = stream;
            error_output_ = stream;
            fatal_output_ = stream;
            break;
        case Level::WARNING:
            debug_output_ = ignore;
            info_output_ = ignore;
            warning_output_ = stream;
            error_output_ = stream;
            fatal_output_ = stream;
            break;
        case Level::ERROR:
            debug_output_ = ignore;
            info_output_ = ignore;
            warning_output_ = ignore;
            error_output_ = stream;
            fatal_output_ = stream;
            break;
        case Level::FATAL:
            debug_output_ = ignore;
            info_output_ = ignore;
            warning_output_ = ignore;
            error_output_ = ignore;
            fatal_output_ = stream;
            break;
        default:
            debug_output_ = ignore;
            info_output_ = ignore;
            warning_output_ = ignore;
            error_output_ = ignore;
            fatal_output_ = ignore;
            break;
    }
}

LoggerFactory::LoggerFactory() : ignore_(), outputs_(), streams_() {
}

LoggerFactory& LoggerFactory::instance() {
    static LoggerFactory instance_;
    return instance_;
}

bool LoggerFactory::configure(const Id& id, Level level, std::ostream* stream, bool owned) {
    if (streams_.find(stream) == streams_.end()) {
        streams_.emplace(stream, owned);
    }
    auto found_output = outputs_.find(id);
    Output *output;
    bool is_new = found_output == outputs_.end();
    if (is_new) {
        output = new Output{*this, id};
        outputs_.emplace(id, output);
    } else {
        output = found_output->second;
    }
    output->configure(level, stream);
    return is_new;
}

Logger LoggerFactory::create_logger(const Id& id) {
    auto found_output = outputs_.find(id);
    Output *output;
    if (found_output == outputs_.end()) {
        output = new Output{*this, id};
        outputs_.emplace(id, output);
    } else {
        output = found_output->second;
    }
    return Logger{output};
}

ostream* LoggerFactory::ignore() {
    return &ignore_;
}

LoggerFactory::~LoggerFactory() {
    for (auto entry : outputs_) {
        delete entry.second;
    }
    for (auto entry : streams_) {
        if (entry.second) {
            delete entry.first;
        }
    }
}

Logger::Logger(const Output* output) : output_(output) {
}

bool Game::Log::configure_logger(const Id& id, Level level, std::ostream* output, bool owned) {
    return LoggerFactory::instance().configure(id, level, output, owned);
}

Logger Game::Log::create_logger(const Id& id) {
    return LoggerFactory::instance().create_logger(id);
};

ostream::int_type IgnoreStreamBuffer::overflow(ostream::int_type c) {
    setp(buffer_, buffer_ + sizeof (buffer_));
    if (c == ostream::traits_type::eof()) {
        return '\0';
    } else {
        return c;
    }
};

IgnoreStream::IgnoreStream() : IgnoreStreamBuffer(), ostream {this}{
}