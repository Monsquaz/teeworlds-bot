#include "StickToXPositionStrategy.h"

StickToXPositionStrategy::StickToXPositionStrategy(float targetX) {
	this->targetX = targetX;
}

void StickToXPositionStrategy::execute(CCharacterCore *player, CControls *controls) {
	const float sensitivity = 3;
	const float veloFactor = 1.5;
	
	float delta = player->m_Pos.x - this->targetX;
	float expectedNextDelta = delta + player->m_Vel.x * veloFactor;


	printf("%.2f + %.2f = %.2f\n", delta, player->m_Vel.x, expectedNextDelta);

	controls->m_InputDirectionRight = 0;
	controls->m_InputDirectionLeft = 0;

	if (delta * expectedNextDelta < 0 && fabs(expectedNextDelta) < sensitivity * 2) {
		//different signs AND not too severe movement
		return;
	}

	if (expectedNextDelta < -sensitivity) {
		controls->m_InputDirectionRight = 1;
	} else if (expectedNextDelta > sensitivity) {
		controls->m_InputDirectionLeft = 1;
	}

}
