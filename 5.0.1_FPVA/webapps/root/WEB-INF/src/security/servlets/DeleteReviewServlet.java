package security.servlets;

import java.io.IOException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import java.sql.SQLException;

import security.helper.SqlQuery;
import security.helper.CookieHelper;

/**
 * @author Evan Kivolowitz
 */
public class DeleteReviewServlet extends HttpServlet {

    @Override
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // Error checking
        if (!CookieHelper.checkCookies(request)) {
            System.out.println("Cookies are invalid");
            response.sendRedirect("/index.html");
            return;
        }

        String username = CookieHelper.getUsernameCookieName(request);
        if (username == null) {
            System.out.println("Session is not null but username is");
            response.sendRedirect("/index.html");
            return;
        }

        String id = (String) request.getParameter("reviewID");
        if (id.equals("")) {
            response.sendRedirect("/home");
            return;
        }
        // end error checking

        SqlQuery sql = new SqlQuery();
        try {
            String SqlQuery = "DELETE FROM REVIEWS WHERE reviewID = ? AND reviewer = ?";
            String[] values = {id, username};
            sql.delete(SqlQuery, values);
            response.sendRedirect("/home");
        } catch (SQLException e) {
            request.setAttribute("error", e.toString());
            request.getRequestDispatcher("/WEB-INF/jsp/error.jsp").forward(request, response);
        }
        sql.close();
    }
}
