import java.io.IOException;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

/**
 * Listen for get requests with sessions attached. Count the number of "clicks"
 * for each session.
 * 
 * @author eichenhofer
 */
public class DoActionServlet extends HttpServlet {

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.servlet.http.HttpServlet#doGet(javax.servlet.http.HttpServletRequest,
	 * javax.servlet.http.HttpServletResponse)
	 * 
	 * check for HTTP session and register a click if one exists; print to std out
	 */
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse res) throws IOException {
		HttpSession session = req.getSession(false);

		if (session == null) { // check whether session exists (whether logged in)
			res.sendError(HttpServletResponse.SC_BAD_REQUEST, "You need to login first"); // error message
			return;
		}

		String expectedToken = (String) session.getAttribute("token"); // get expected token from session
		String actualToken = req.getParameter("token"); // get actual token from parameter

		// Check whether both expected token and actual token both exist and are equal
		if (expectedToken == null || actualToken == null || !expectedToken.equals(actualToken)) {
			res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Invalid token"); // error message
			return;
		}

		if (session != null) {
			Integer currCount = (Integer) session.getAttribute("clicks");
			session.setAttribute("clicks", currCount + 1);
			System.out.println(session.getAttribute("username") + " clicked!");
		} else {
			System.out.println("no-session clicked");
		}

		res.sendRedirect("view");
	}
}