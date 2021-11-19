import java.util.concurrent.*;
import java.util.concurrent.locks.*;
import java.util.concurrent.locks.Condition.*;

public class TrafficController {
    final Lock trava = new ReentrantLock();
    final Condition Red = trava.newCondition();
    final Condition Blue = trava.newCondition();

    // via = 0 -> s/ carro na ponte
    // via > 0 -> vermelhos passando
    // via < 0 -> azuis passando
    int via = 0;

    // (RED: left --> x --- right)
    // entra na SC
    public void enterLeft() {
        trava.lock();
        try{
            if(via < 0)
                Red.await();
        } catch(InterruptedException ie){
            System.err.println(ie.toString());
        }
        via++;
        trava.unlock();
    }

    // (RED: left left --- x --> right)
    // sai da SC
    public void leaveRight() {
        trava.lock();
        via--;
        if(via == 0)
            Blue.signalAll();
        trava.unlock();
    }

    // (BLUE: left --- x <-- right)
    // entra na SC
    public void enterRight() {
        trava.lock();
        try{
            if(via > 0)
                Blue.await();
        } catch(InterruptedException ie){
            System.err.println(ie.toString());
        }
        via--;
        trava.unlock();
    }

    // (BLUE: left <-- x --- right)
    // sai da SC
    public void leaveLeft() {
        trava.lock();
        via++;
        if(via == 0)
            Red.signalAll();
        trava.unlock();
    }
}