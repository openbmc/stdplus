#pragma once

namespace stdplus
{
namespace signal
{

/** @brief Blocks the signal from being handled by the designated
 *         sigaction. If the signal is already blocked this does nothing.
 *
 *  @param[in] signum - The int representing the signal to block
 *  @throws std::system_error if any underlying error occurs.
 */
void block(int signum);

/** @brief Handle the SIGSEGV signal and print out the stack trace before
 * exiting the program.
 */
void stackTrace();

} // namespace signal
} // namespace stdplus
