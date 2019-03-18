import java.io.IOException;
import java.security.SecureRandom; // used to generate random number 
import java.math.BigInteger; // used to convert generated random number to String

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Java servlet for creating a session and attaching two attributes: a username
 * and a counter. Does not consider existing sessions.
 * 
 * @author Joseph Eichenhofer
 */
public class LoginServlet extends HttpServlet {
	static private SecureRandom random = new SecureRandom(); // random number generator
	static private byte[] tokenByteArray = new byte[32]; // byte array used to generate token

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.servlet.http.HttpServlet#doGet(javax.servlet.http.HttpServletRequest,
	 * javax.servlet.http.HttpServletResponse)
	 * 
	 * Create a session for the specified username (no checks of
	 * password/authentication). Does not consider existing sessions with same
	 * username. Initialize clicks to zero.
	 */
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse res) throws IOException {
		// check for username parameter
		String username = req.getParameter("username");
		if (username != null && !username.equals("")) {
			// request contained a username, set session attribute for username and
			// initialize click count to zero
			req.getSession(true).setAttribute("username", username);
			random.nextBytes(tokenByteArray); // generate random bytes and save to tokenByteArray
			String token = new BigInteger(tokenByteArray).toString(16); // convert to hex string
			req.getSession().setAttribute("token", token); // save generated token to session
			req.getSession().setAttribute("clicks", new Integer(0));
		}

		// redirect to main page
		res.sendRedirect("view");
	}
}
